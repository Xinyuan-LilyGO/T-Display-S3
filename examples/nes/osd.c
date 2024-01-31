/* start rewrite from: https://github.com/espressif/esp32-nesemu.git */
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <driver/i2s.h>
#include <esp_heap_caps.h>

#include <noftypes.h>

#include <event.h>
#include <gui.h>
#include <log.h>
#include <nes/nes.h>
#include <nes/nes_pal.h>
#include <nes/nesinput.h>
#include <nofconfig.h>
#include <osd.h>


TimerHandle_t timer;

/* memory allocation */
extern void *mem_alloc(int size, bool prefer_fast_memory)
{
    if (prefer_fast_memory) {
        return heap_caps_malloc(size, MALLOC_CAP_8BIT);
    } else {
        return heap_caps_malloc_prefer(size, MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT);
    }
}

/* audio */
#define DEFAULT_SAMPLERATE 22050

#if defined(HW_AUDIO)

#define DEFAULT_FRAGSIZE 1024
static void (*audio_callback)(void *buffer, int length) = NULL;
QueueHandle_t queue;
static int16_t *audio_frame;

static int osd_init_sound(void)
{
    audio_frame = NOFRENDO_MALLOC(4 * DEFAULT_FRAGSIZE);

    i2s_config_t cfg = {
#if defined(HW_AUDIO_EXTDAC)
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
#else  /* !defined(HW_AUDIO_EXTDAC) */
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
#endif /* !defined(HW_AUDIO_EXTDAC) */
        .sample_rate = DEFAULT_SAMPLERATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
#if defined(HW_AUDIO_EXTDAC)
        .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
#else  /* !defined(HW_AUDIO_EXTDAC) */
        .communication_format = I2S_COMM_FORMAT_PCM | I2S_COMM_FORMAT_I2S_MSB,
#endif /* !defined(HW_AUDIO_EXTDAC) */
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 6,
        .dma_buf_len = 512,
        .use_apll = false,
    };
    i2s_driver_install(I2S_NUM_0, &cfg, 2, &queue);
#if defined(HW_AUDIO_EXTDAC)
    i2s_pin_config_t pins = {
        .bck_io_num = HW_AUDIO_EXTDAC_BCLK,
        .ws_io_num = HW_AUDIO_EXTDAC_WCLK,
        .data_out_num = HW_AUDIO_EXTDAC_DOUT,
        .data_in_num = I2S_PIN_NO_CHANGE,
    };
    i2s_set_pin(I2S_NUM_0, &pins);
#else  /* !defined(HW_AUDIO_EXTDAC) */
    i2s_set_pin(I2S_NUM_0, NULL);
    i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
#endif /* !defined(HW_AUDIO_EXTDAC) */
    i2s_zero_dma_buffer(I2S_NUM_0);

    audio_callback = NULL;

    return 0;
}

static void osd_stopsound(void)
{
    audio_callback = NULL;
}

static void do_audio_frame()
{
    int left = DEFAULT_SAMPLERATE / NES_REFRESH_RATE;
    while (left) {
        int n = DEFAULT_FRAGSIZE;
        if (n > left)
            n = left;
        audio_callback(audio_frame, n); //get more data

        //16 bit mono -> 32-bit (16 bit r+l)
        int16_t *mono_ptr = audio_frame + n;
        int16_t *stereo_ptr = audio_frame + n + n;
        int i = n;
        while (i--) {
#if defined(HW_AUDIO_EXTDAC)
            int16_t a = (*(--mono_ptr) >> 2);
            *(--stereo_ptr) = a;
            *(--stereo_ptr) = a;
#else  /* !defined(HW_AUDIO_EXTDAC) */
            int16_t a = (*(--mono_ptr) >> 3);
            *(--stereo_ptr) = 0x8000 + a;
            *(--stereo_ptr) = 0x8000 - a;
#endif /* !defined(HW_AUDIO_EXTDAC) */
        }

        size_t i2s_bytes_write;
        i2s_write(I2S_NUM_0, (const char *)audio_frame, 4 * n, &i2s_bytes_write, portMAX_DELAY);
        left -= i2s_bytes_write / 4;
    }
}

void osd_setsound(void (*playfunc)(void *buffer, int length))
{
    //Indicates we should call playfunc() to get more data.
    audio_callback = playfunc;
}

#else /* !defined(HW_AUDIO) */

static int osd_init_sound(void)
{
    return 0;
}

static void osd_stopsound(void)
{
}

static void do_audio_frame()
{
}

void osd_setsound(void (*playfunc)(void *buffer, int length))
{
}

#endif /* !defined(HW_AUDIO) */

/* video */
extern void display_init();
extern void display_write_frame(const uint8_t *data[]);
extern void display_clear();

//This runs on core 0.
QueueHandle_t vidQueue;
static void videoTask(void *arg)
{
    bitmap_t *bmp = NULL;
    while (1) {
        // xQueueReceive(vidQueue, &bmp, portMAX_DELAY); //skip one frame to drop to 30
        xQueueReceive(vidQueue, &bmp, portMAX_DELAY);
        display_write_frame((const uint8_t **)bmp->line);
    }
}

/* get info */
static char fb[1]; //dummy
bitmap_t *myBitmap;

/* initialise video */
static int init(int width, int height)
{
    return 0;
}

static void shutdown(void)
{
}

/* set a video mode */
static int set_mode(int width, int height)
{
    return 0;
}

/* copy nes palette over to hardware */
uint16 myPalette[256];
static void set_palette(rgb_t *pal)
{
    uint16 c;

    int i;

    for (i = 0; i < 256; i++) {
        c = (pal[i].b >> 3) + ((pal[i].g >> 2) << 5) + ((pal[i].r >> 3) << 11);
        //myPalette[i]=(c>>8)|((c&0xff)<<8);
        myPalette[i] = c;
    }
}

/* clear all frames to a particular color */
static void clear(uint8 color)
{
    // SDL_FillRect(mySurface, 0, color);
    display_clear();
}

/* acquire the directbuffer for writing */
static bitmap_t *lock_write(void)
{
    // SDL_LockSurface(mySurface);
    myBitmap = bmp_createhw((uint8 *)fb, NES_SCREEN_WIDTH, NES_SCREEN_HEIGHT, NES_SCREEN_WIDTH * 2);
    return myBitmap;
}

/* release the resource */
static void free_write(int num_dirties, rect_t *dirty_rects)
{
    bmp_destroy(&myBitmap);
}

static void custom_blit(bitmap_t *bmp, int num_dirties, rect_t *dirty_rects)
{
    xQueueSend(vidQueue, &bmp, 0);
    do_audio_frame();
}

viddriver_t sdlDriver = {
    "Simple DirectMedia Layer", /* name */
    init,                       /* init */
    shutdown,                   /* shutdown */
    set_mode,                   /* set_mode */
    set_palette,                /* set_palette */
    clear,                      /* clear */
    lock_write,                 /* lock_write */
    free_write,                 /* free_write */
    custom_blit,                /* custom_blit */
    false                       /* invalidate flag */
};

void osd_getvideoinfo(vidinfo_t *info)
{
    info->default_width = NES_SCREEN_WIDTH;
    info->default_height = NES_SCREEN_HEIGHT;
    info->driver = &sdlDriver;
}

void osd_getsoundinfo(sndinfo_t *info)
{
    info->sample_rate = DEFAULT_SAMPLERATE;
    info->bps = 16;
}

/* input */
extern void controller_init();
extern uint32_t controller_read_input();

static void osd_initinput()
{
    controller_init();
}

static void osd_freeinput(void)
{
}

void osd_getinput(void)
{
    const int ev[32] = {
        event_joypad1_up, event_joypad1_down, event_joypad1_left, event_joypad1_right,
        event_joypad1_select, event_joypad1_start, event_joypad1_a, event_joypad1_b,
        event_state_save, event_state_load, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };
    static int oldb = 0xffff;
    uint32_t b = controller_read_input();
    uint32_t chg = b ^ oldb;
    int x;
    oldb = b;
    event_t evh;
    // nofrendo_log_printf("Input: %x\n", b);
    for (x = 0; x < 16; x++) {
        if (chg & 1) {
            evh = event_get(ev[x]);
            if (evh)
                evh((b & 1) ? INP_STATE_BREAK : INP_STATE_MAKE);
        }
        chg >>= 1;
        b >>= 1;
    }
}

void osd_getmouse(int *x, int *y, int *button)
{
}

/* init / shutdown */
static int logprint(const char *string)
{
    return printf("%s", string);
}

int osd_init()
{
    // nofrendo_log_chain_logfunc(logprint);

    if (osd_init_sound())
        return -1;

    display_init();
    vidQueue = xQueueCreate(1, sizeof(bitmap_t *));
    xTaskCreatePinnedToCore(&videoTask, "videoTask", 2048, NULL, 0, NULL, 0);
    osd_initinput();
    return 0;
}

void osd_shutdown()
{
    osd_stopsound();
    osd_freeinput();
}

char configfilename[] = "na";
int osd_main(int argc, char *argv[])
{
    config.filename = configfilename;

    return main_loop(argv[0], system_autodetect);
}

//Seemingly, this will be called only once. Should call func with a freq of frequency,
int osd_installtimer(int frequency, void *func, int funcsize, void *counter, int countersize)
{
    nofrendo_log_printf("Timer install, configTICK_RATE_HZ=%d, freq=%d\n", configTICK_RATE_HZ, frequency);
    timer = xTimerCreate("nes", configTICK_RATE_HZ / frequency, pdTRUE, NULL, func);
    xTimerStart(timer, 0);
    return 0;
}

/* filename manipulation */
void osd_fullname(char *fullname, const char *shortname)
{
    strncpy(fullname, shortname, PATH_MAX);
}

/* This gives filenames for storage of saves */
char *osd_newextension(char *string, char *ext)
{
    // dirty: assume both extensions is 3 characters
    size_t l = strlen(string);
    string[l - 3] = ext[1];
    string[l - 2] = ext[2];
    string[l - 1] = ext[3];

    return string;
}

/* This gives filenames for storage of PCX snapshots */
int osd_makesnapname(char *filename, int len)
{
    return -1;
}
