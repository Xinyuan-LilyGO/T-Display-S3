/* Please make sure your touch IC model. */
// If you don't have a touch panel, you don't need to define a touch model

// 定义一个触摸型号，如果不清楚你使用的触摸型号，可以任意定义一个触摸型号，将下面两行中的一个取消掉，如果触摸不正常，那就换另一个试试
// 如果不定义,则禁用触摸功能
// #define TOUCH_MODULES_CST_MUTUAL
// #define TOUCH_MODULES_CST_SELF

#if defined(TOUCH_MODULES_CST_MUTUAL) ||defined(TOUCH_MODULES_CST_SELF)
#include "TouchLib.h"
#endif
// #define TOUCH_READ_FROM_INTERRNUPT

/* The product now has two screens, and the initialization code needs a small change in the new version. The LCD_MODULE_CMD_1 is used to define the
 * switch macro. */
#define LCD_MODULE_CMD_1

#include "OneButton.h" /* https://github.com/mathertel/OneButton.git */
#include "lvgl.h"      /* https://github.com/lvgl/lvgl.git */

#include "Arduino.h"
#include "SD_MMC.h"
#include "WiFi.h"
#include "Wire.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "factory_gui.h"
#include "pin_config.h"
#include "esp_sntp.h"
#include "time.h"

#include "zones.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <esp_wifi.h>

// The factory program uses the Chinese time zone by default.
// Commenting this line will automatically get the time zone, provided that the SSL certificate is valid.
// Please pay attention to check the validity of the certificate.
// The current configuration certificate is valid until Mar 29 22:40:08 2025 GMT

// #define CUSTOM_TIMEZONE         "CST-8"


#ifndef CUSTOM_TIMEZONE
// Use command get server ca
// openssl s_client -connect ipapi.co:443 -showcerts </dev/null 2> /dev/null | sed -n '/BEGIN/,/END/p' > server.pem

// Check the validity period of the certificate
// openssl s_client -connect ipapi.co:443 -servername ipapi.co 2>/dev/null | openssl x509 -noout -dates
static const char *rootCACertificate PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDmjCCA0GgAwIBAgIQKNsg9z6tYmQOV0wv+VFSyTAKBggqhkjOPQQDAjA7MQsw
CQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZpY2VzMQwwCgYD
VQQDEwNXRTEwHhcNMjQxMjI5MjE0MDEwWhcNMjUwMzI5MjI0MDA4WjATMREwDwYD
VQQDEwhpcGFwaS5jbzBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABEfuWuJphWh5
CzoKqMJvfBA4v977Vu79l4P5aCtOPVPVBvNTxzdLwMvMp2XVas46AVCm/B4PODlw
PxeL6BzYYMSjggJNMIICSTAOBgNVHQ8BAf8EBAMCB4AwEwYDVR0lBAwwCgYIKwYB
BQUHAwEwDAYDVR0TAQH/BAIwADAdBgNVHQ4EFgQU/2QCuuN/r4UoC67WpNau15Ex
i4YwHwYDVR0jBBgwFoAUkHeSNWfE/6jMqeZ72YB5e8yT+TgwXgYIKwYBBQUHAQEE
UjBQMCcGCCsGAQUFBzABhhtodHRwOi8vby5wa2kuZ29vZy9zL3dlMS9LTnMwJQYI
KwYBBQUHMAKGGWh0dHA6Ly9pLnBraS5nb29nL3dlMS5jcnQwHwYDVR0RBBgwFoII
aXBhcGkuY2+CCiouaXBhcGkuY28wEwYDVR0gBAwwCjAIBgZngQwBAgEwNgYDVR0f
BC8wLTAroCmgJ4YlaHR0cDovL2MucGtpLmdvb2cvd2UxL2NNNzZRWlVYbUU4LmNy
bDCCAQQGCisGAQQB1nkCBAIEgfUEgfIA8AB2AE51oydcmhDDOFts1N8/Uusd8OCO
G41pwLH6ZLFimjnfAAABlBSTr2QAAAQDAEcwRQIhAIvH87A7Qo7VMPFXxf7M2nyD
hvNFSHLNDXtPyRFX5d/cAiA+KbvJSSNo4A5tfnn2nQ4d3j0amEv1pvK53rhqhtD0
kgB2AOCSs/wMHcjnaDYf3mG5lk0KUngZinLWcsSwTaVtb1QEAAABlBSTr4AAAAQD
AEcwRQIhANvKZUFQP5HFqoRLfDyTmrgrqTRVlHOYSZB8o5FBFjkmAiA/7S/vCPIW
tY9gHN7YlhvpxwXo3kvyK3UPTLyv2oVPJjAKBggqhkjOPQQDAgNHADBEAiBchwgj
Iv79+O7ETpt63+rEFuKfITJRW7ly9yrYlzVCxwIga/EwDlz3XTdSQ5vug5Q7JlZF
EHuBT3euPeKXDamodqo=
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIICnzCCAiWgAwIBAgIQf/MZd5csIkp2FV0TttaF4zAKBggqhkjOPQQDAzBHMQsw
CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU
MBIGA1UEAxMLR1RTIFJvb3QgUjQwHhcNMjMxMjEzMDkwMDAwWhcNMjkwMjIwMTQw
MDAwWjA7MQswCQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZp
Y2VzMQwwCgYDVQQDEwNXRTEwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARvzTr+
Z1dHTCEDhUDCR127WEcPQMFcF4XGGTfn1XzthkubgdnXGhOlCgP4mMTG6J7/EFmP
LCaY9eYmJbsPAvpWo4H+MIH7MA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggr
BgEFBQcDAQYIKwYBBQUHAwIwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQU
kHeSNWfE/6jMqeZ72YB5e8yT+TgwHwYDVR0jBBgwFoAUgEzW63T/STaj1dj8tT7F
avCUHYwwNAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzAChhhodHRwOi8vaS5wa2ku
Z29vZy9yNC5jcnQwKwYDVR0fBCQwIjAgoB6gHIYaaHR0cDovL2MucGtpLmdvb2cv
ci9yNC5jcmwwEwYDVR0gBAwwCjAIBgZngQwBAgEwCgYIKoZIzj0EAwMDaAAwZQIx
AOcCq1HW90OVznX+0RGU1cxAQXomvtgM8zItPZCuFQ8jSBJSjz5keROv9aYsAm5V
sQIwJonMaAFi54mrfhfoFNZEfuNMSQ6/bIBiNLiyoX46FohQvKeIoJ99cx7sUkFN
7uJW
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIIDejCCAmKgAwIBAgIQf+UwvzMTQ77dghYQST2KGzANBgkqhkiG9w0BAQsFADBX
MQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBudi1zYTEQMA4GA1UE
CxMHUm9vdCBDQTEbMBkGA1UEAxMSR2xvYmFsU2lnbiBSb290IENBMB4XDTIzMTEx
NTAzNDMyMVoXDTI4MDEyODAwMDA0MlowRzELMAkGA1UEBhMCVVMxIjAgBgNVBAoT
GUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBMTEMxFDASBgNVBAMTC0dUUyBSb290IFI0
MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE83Rzp2iLYK5DuDXFgTB7S0md+8Fhzube
Rr1r1WEYNa5A3XP3iZEwWus87oV8okB2O6nGuEfYKueSkWpz6bFyOZ8pn6KY019e
WIZlD6GEZQbR3IvJx3PIjGov5cSr0R2Ko4H/MIH8MA4GA1UdDwEB/wQEAwIBhjAd
BgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDwYDVR0TAQH/BAUwAwEB/zAd
BgNVHQ4EFgQUgEzW63T/STaj1dj8tT7FavCUHYwwHwYDVR0jBBgwFoAUYHtmGkUN
l8qJUC99BM00qP/8/UswNgYIKwYBBQUHAQEEKjAoMCYGCCsGAQUFBzAChhpodHRw
Oi8vaS5wa2kuZ29vZy9nc3IxLmNydDAtBgNVHR8EJjAkMCKgIKAehhxodHRwOi8v
Yy5wa2kuZ29vZy9yL2dzcjEuY3JsMBMGA1UdIAQMMAowCAYGZ4EMAQIBMA0GCSqG
SIb3DQEBCwUAA4IBAQAYQrsPBtYDh5bjP2OBDwmkoWhIDDkic574y04tfzHpn+cJ
odI2D4SseesQ6bDrarZ7C30ddLibZatoKiws3UL9xnELz4ct92vID24FfVbiI1hY
+SW6FoVHkNeWIP0GCbaM4C6uVdF5dTUsMVs/ZbzNnIdCp5Gxmx5ejvEau8otR/Cs
kGN+hr/W5GvT1tMBjgWKZ1i4//emhA1JG1BbPzoLJQvyEotc03lXjTaCzv8mEbep
8RqZ7a2CPsgRbuvTPBwcOMBBmuFeU88+FSBX6+7iP0il8b4Z0QFqIwwMHfs/L6K1
vepuoxtGzi4CZ68zJpiq1UvSqTbFJjtbD4seiMHl
-----END CERTIFICATE-----
)EOF";
#endif


esp_lcd_panel_io_handle_t io_handle = NULL;
static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
static lv_disp_drv_t disp_drv;      // contains callback functions
static lv_color_t *lv_disp_buf;
static bool is_initialized_lvgl = false;
OneButton button1(PIN_BUTTON_1, true);
OneButton button2(PIN_BUTTON_2, true);
#if defined(LCD_MODULE_CMD_1)
typedef struct {
    uint8_t cmd;
    uint8_t data[14];
    uint8_t len;
} lcd_cmd_t;

lcd_cmd_t lcd_st7789v[] = {
    {0x11, {0}, 0 | 0x80},
    {0x3A, {0X05}, 1},
    {0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5},
    {0xB7, {0X75}, 1},
    {0xBB, {0X28}, 1},
    {0xC0, {0X2C}, 1},
    {0xC2, {0X01}, 1},
    {0xC3, {0X1F}, 1},
    {0xC6, {0X13}, 1},
    {0xD0, {0XA7}, 1},
    {0xD0, {0XA4, 0XA1}, 2},
    {0xD6, {0XA1}, 1},
    {0xE0, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14},
    {0xE1, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14},

};
#endif

#if defined(TOUCH_MODULES_CST_MUTUAL)
TouchLib touch(Wire, PIN_IIC_SDA, PIN_IIC_SCL, CTS328_SLAVE_ADDRESS, PIN_TOUCH_RES);
#elif defined(TOUCH_MODULES_CST_SELF)
TouchLib touch(Wire, PIN_IIC_SDA, PIN_IIC_SCL, CTS820_SLAVE_ADDRESS, PIN_TOUCH_RES);
#else
#warning "Touch models are not currently configured, use the sketch without touch panel"
#endif

bool inited_touch = false;
bool inited_sd = false;
#if defined(TOUCH_READ_FROM_INTERRNUPT)
bool get_int_signal = false;
#endif

void wifi_test(void);
void timeavailable(struct timeval *t);
void printLocalTime();
void SmartConfig();
void setTimezone();

static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    if (is_initialized_lvgl) {
        lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
        lv_disp_flush_ready(disp_driver);
    }
    return false;
}

static void example_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // copy a buffer's content to a specific area of the display
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

#if defined(TOUCH_MODULES_CST_MUTUAL) ||defined(TOUCH_MODULES_CST_SELF)
static void lv_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
#if defined(TOUCH_READ_FROM_INTERRNUPT)
    if (get_int_signal) {
        get_int_signal = false;
        touch.read();
#else
    if (touch.read()) {
#endif
        String str_buf;
        uint8_t fn = touch.getPointNum();
        str_buf += " Finger num : " + String(fn) + " \n";
        for (uint8_t i = 0; i < fn; i++) {
            TP_Point t = touch.getPoint(i);
            str_buf += "x: " + String(t.x) + " y: " + String(t.y) + " p: " + String(t.pressure) + " \n";
        }
        TP_Point t = touch.getPoint(0);
        data->point.x = t.x;
        data->point.y = t.y;
        data->state = LV_INDEV_STATE_PR;
        lv_msg_send(MSG_NEW_TOUCH_POINT, str_buf.c_str());
    } else
        data->state = LV_INDEV_STATE_REL;
}
#endif


// LilyGo  T-Display-S3  control backlight chip has 16 levels of adjustment range
// The adjustable range is 0~16, 0 is the minimum brightness, 16 is the maximum brightness
void setBrightness(uint8_t value)
{
    static uint8_t level = 0;
    static uint8_t steps = 16;
    if (value == 0) {
        digitalWrite(PIN_LCD_BL, 0);
        delay(3);
        level = 0;
        return;
    }
    if (level == 0) {
        digitalWrite(PIN_LCD_BL, 1);
        level = steps;
        delayMicroseconds(30);
    }
    int from = steps - level;
    int to = steps - value;
    int num = (steps + to - from) % steps;
    for (int i = 0; i < num; i++) {
        digitalWrite(PIN_LCD_BL, 0);
        digitalWrite(PIN_LCD_BL, 1);
    }
    level = value;
}

void setup()
{
    // (POWER ON)IO15 must be set to HIGH before starting, otherwise the screen will not display when using battery
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    Serial.begin(115200);

    sntp_servermode_dhcp(1); // (optional)
    configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);

    pinMode(PIN_LCD_RD, OUTPUT);
    digitalWrite(PIN_LCD_RD, HIGH);
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = PIN_LCD_DC,
        .wr_gpio_num = PIN_LCD_WR,
        .clk_src = LCD_CLK_SRC_PLL160M,
        .data_gpio_nums =
        {
            PIN_LCD_D0,
            PIN_LCD_D1,
            PIN_LCD_D2,
            PIN_LCD_D3,
            PIN_LCD_D4,
            PIN_LCD_D5,
            PIN_LCD_D6,
            PIN_LCD_D7,
        },
        .bus_width = 8,
        .max_transfer_bytes = LVGL_LCD_BUF_SIZE * sizeof(uint16_t),
        .psram_trans_align = 0,
        .sram_trans_align = 0
    };
    esp_lcd_new_i80_bus(&bus_config, &i80_bus);

    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = PIN_LCD_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 20,
        .on_color_trans_done = example_notify_lvgl_flush_ready,
        .user_ctx = &disp_drv,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_levels =
        {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_LCD_RES,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
        .vendor_config = NULL
    };
    esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle);
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);

    esp_lcd_panel_invert_color(panel_handle, true);

    esp_lcd_panel_swap_xy(panel_handle, true);

    //The screen faces you, and the USB is on the left
    esp_lcd_panel_mirror(panel_handle, false, true);

    //The screen faces you, the USB is to the right
    // esp_lcd_panel_mirror(panel_handle, true, false);

    // the gap is LCD panel specific, even panels with the same driver IC, can
    // have different gap value
    esp_lcd_panel_set_gap(panel_handle, 0, 35);
#if defined(LCD_MODULE_CMD_1)
    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++) {
        esp_lcd_panel_io_tx_param(io_handle, lcd_st7789v[i].cmd, lcd_st7789v[i].data, lcd_st7789v[i].len & 0x7f);
        if (lcd_st7789v[i].len & 0x80)
            delay(120);
    }
#endif
    lv_init();
    lv_disp_buf = (lv_color_t *)heap_caps_malloc(LVGL_LCD_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

    lv_disp_draw_buf_init(&disp_buf, lv_disp_buf, NULL, LVGL_LCD_BUF_SIZE);
    /*Initialize the display*/
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = example_lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
    lv_disp_drv_register(&disp_drv);

#if defined(TOUCH_MODULES_CST_MUTUAL) || defined(TOUCH_MODULES_CST_SELF)
    /* Register touch brush with LVGL */
    // Wire.begin(PIN_IIC_SDA, PIN_IIC_SCL, 800000);
    inited_touch = touch.init();
    if (inited_touch) {
        touch.setRotation(1);
        static lv_indev_drv_t indev_drv;
        lv_indev_drv_init(&indev_drv);
        indev_drv.type = LV_INDEV_TYPE_POINTER;
        indev_drv.read_cb = lv_touchpad_read;
        lv_indev_drv_register(&indev_drv);
    }
#if defined(TOUCH_READ_FROM_INTERRNUPT)
    attachInterrupt(
        PIN_TOUCH_INT, [] { get_int_signal = true; }, FALLING);
#endif
#endif

    is_initialized_lvgl = true;

    LV_IMG_DECLARE(lilygo2_gif);
    lv_obj_t *logo_img = lv_gif_create(lv_scr_act());
    lv_obj_center(logo_img);
    lv_gif_set_src(logo_img, &lilygo2_gif);

    // Adjust brightness
    pinMode(PIN_LCD_BL, OUTPUT);
    // Brightness range : 0 ~ 16 level
    for (int i = 0; i <= 16; ++i) {
        setBrightness(i);
        lv_timer_handler();
        delay(50);
    }

    LV_DELAY(1200);
    lv_obj_del(logo_img);

    // https://github.com/Xinyuan-LilyGO/T-Display-S3/issues/278
    Serial.println("tone ...");
    tone(10, 1000, 5000);
    delay(5000);
    Serial.println("tone done .");


    SD_MMC.setPins(PIN_SD_CLK, PIN_SD_CMD, PIN_SD_D0);
    inited_sd = SD_MMC.begin("/sdcard", true, true);

    wifi_test();

    button1.attachClick([]() {
        // Sleep display
        esp_lcd_panel_io_tx_param(io_handle, 0x10, NULL, 0);
        pinMode(PIN_POWER_ON, OUTPUT);
        pinMode(PIN_LCD_BL, OUTPUT);
        digitalWrite(PIN_POWER_ON, LOW);
        digitalWrite(PIN_LCD_BL, LOW);
        esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_BUTTON_2, 0); // 1 = High, 0 = Low
        esp_deep_sleep_start();
    });

    button2.attachClick([]() {
        ui_switch_page();
    });
}

void loop()
{
    lv_timer_handler();
    button1.tick();
    button2.tick();
    delay(3);
    static uint32_t last_tick;
    if (millis() - last_tick > 100) {
        struct tm timeinfo;
        if (getLocalTime(&timeinfo)) {
            lv_msg_send(MSG_NEW_HOUR, &timeinfo.tm_hour);
            lv_msg_send(MSG_NEW_MIN, &timeinfo.tm_min);
        }
        uint32_t volt = (analogRead(PIN_BAT_VOLT) * 2 * 3.3 * 1000) / 4096;
        lv_msg_send(MSG_NEW_VOLT, &volt);

        last_tick = millis();
    }
}


void wifi_test(void)
{
    String text;

    lv_obj_t *log_label = lv_label_create(lv_scr_act());
    lv_obj_align(log_label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_width(log_label, LV_PCT(100));
    lv_label_set_long_mode(log_label, LV_LABEL_LONG_SCROLL);
    lv_label_set_recolor(log_label, true);

    lv_label_set_text(log_label, "Scan WiFi");
    LV_DELAY(1);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        text = "no networks found";
    } else {
        text = n;
        text += " networks found\n";
        for (int i = 0; i < n; ++i) {
            text += (i + 1);
            text += ": ";
            text += WiFi.SSID(i);
            text += " (";
            text += WiFi.RSSI(i);
            text += ")";
            text += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " \n" : "*\n";
            delay(10);
        }
    }

    wifi_config_t current_conf;
    esp_wifi_get_config(WIFI_IF_STA, &current_conf);
    if (strlen((const char *)current_conf.sta.ssid) == 0) {
        // Just for testing.
        Serial.println("Use default WiFi SSID & PASSWORD!!");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    } else {
        Serial.println("Begin WiFi");
        WiFi.begin();
    }

    lv_label_set_text(log_label, text.c_str());
    Serial.println(text);
    LV_DELAY(2000);
    text = "Connecting to ";
    Serial.print("Connecting to ");
    text += (char *)(current_conf.sta.ssid);
    text += "\n";
    Serial.print((char *)(current_conf.sta.ssid));
    // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    uint32_t last_tick = millis();
    bool is_smartconfig_connect = false;
    lv_label_set_long_mode(log_label, LV_LABEL_LONG_WRAP);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        text += ".";
        lv_label_set_text(log_label, text.c_str());
        LV_DELAY(100);
        if (millis() - last_tick > WIFI_CONNECT_WAIT_MAX) { /* Automatically start smartconfig when connection times out */
            text += "\nConnection timed out, start smartconfig";
            lv_label_set_text(log_label, text.c_str());
            LV_DELAY(100);
            is_smartconfig_connect = true;
            WiFi.mode(WIFI_AP_STA);
            Serial.println("\r\n wait for smartconfig....");
            text += "\r\n wait for smartconfig....";
            text += "\nPlease use #ff0000 EspTouch# Apps to connect to the distribution network";
            lv_label_set_text(log_label, text.c_str());
            WiFi.beginSmartConfig();
            while (1) {
                LV_DELAY(100);
                if (WiFi.smartConfigDone()) {
                    Serial.println("\r\nSmartConfig Success\r\n");
                    Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
                    Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
                    text += "\nSmartConfig Success";
                    text += "\nSSID:";
                    text += WiFi.SSID().c_str();
                    text += "\nPSW:";
                    text += WiFi.psk().c_str();
                    lv_label_set_text(log_label, text.c_str());
                    LV_DELAY(1000);
                    last_tick = millis();
                    break;
                }
            }
        }
    }
    if (!is_smartconfig_connect) {
        text += "\nCONNECTED \nTakes ";
        Serial.print("\n CONNECTED \nTakes ");
        text += millis() - last_tick;
        Serial.print(millis() - last_tick);
        text += " ms\n";
        Serial.println(" millseconds");
        lv_label_set_text(log_label, text.c_str());
    }
    LV_DELAY(2000);
    setTimezone();
    ui_begin();
}

void printLocalTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("No time available (yet)");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval *t)
{
    Serial.println("Got time adjustment from NTP!");
    printLocalTime();
    WiFi.disconnect();
}



void setTimezone()
{
#ifdef CUSTOM_TIMEZONE
    String timezone = CUSTOM_TIMEZONE;
#else
    WiFiClientSecure *client = new WiFiClientSecure;
    String timezone;
    if (client) {
        client->setCACert(rootCACertificate);
        HTTPClient https;
        if (https.begin(*client, GET_TIMEZONE_API)) {
            int httpCode = https.GET();
            if (httpCode > 0) {
                // HTTP header has been send and Server response header has been handled
                Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

                // file found at server
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = https.getString();
                    Serial.println(payload);
                    timezone = payload;
                }
            } else {
                Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
            }
            https.end();
        }
        delete client;
    }
    for (uint32_t i = 0; i < sizeof(zones); i++) {
        if (timezone == "None") {
            timezone = "CST-8";
            break;
        }
        if (timezone == zones[i].name) {
            timezone = zones[i].zones;
            break;
        }
    }
#endif

    Serial.println("timezone : " + timezone);
    setenv("TZ", timezone.c_str(), 1); // set time zone
    tzset();
}