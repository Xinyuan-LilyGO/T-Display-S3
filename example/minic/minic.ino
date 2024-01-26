#ifndef USEPLATFORMIO
#error "This example can only be run on platformIO"
#endif

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#error "The current version is not supported for the time being, please use a version below Arduino ESP32 3.0"
#endif

/* Please make sure your touch IC model. */
// #define TOUCH_MODULES_CST_MUTUAL
// #define TOUCH_MODULES_CST_SELF

#include "TouchLib.h"
// #define TOUCH_READ_FROM_INTERRNUPT

/* The product now has two screens, and the initialization code needs a small change in the new version. The LCD_MODULE_CMD_1 is used to define the
 * switch macro. */
#define LCD_MODULE_CMD_1

/*
This example can only be run on platformIO.
Because Arduino cannot index into the demos directory.
*/
#include "demos/lv_demos.h"
#include "lv_conf.h"
#include "lvgl.h" /* https://github.com/lvgl/lvgl.git */

#include "Arduino.h"
#include "Wire.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "pin_config.h"

#include <Ticker.h>
#include "DHT.h"
#include <WiFi.h>
#include <PID_v1.h>

#define DHT22_PIN 1
#define DHT22_PIN_ROOM 2
#define CURR_INPUT 3

#define rele1 12
#define rele2 13
#define rele_fan 43
#define PWM 10

#define PIN_GPIO_11 11
#define PIN_GPIO_12 12

DHT dht(DHT22_PIN, DHT22);
DHT dht_room(DHT22_PIN_ROOM, DHT22); // pass oneWire to DallasTemperature library

esp_lcd_panel_io_handle_t io_handle = NULL;
static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
static lv_disp_drv_t disp_drv;      // contains callback functions
static lv_color_t *lv_disp_buf;
static bool is_initialized_lvgl = false;
#if defined(LCD_MODULE_CMD_1)
typedef struct
{
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

TouchLib touch(Wire, PIN_IIC_SDA, PIN_IIC_SCL, CTS816S_SLAVE_ADDRESS, PIN_TOUCH_RES);

bool inited_touch = false;
#if defined(TOUCH_READ_FROM_INTERRNUPT)
bool get_int_signal = false;
#endif

#define LV_COLOR_RED LV_COLOR_MAKE(0xFF, 0x00, 0x00)
#define LV_COLOR_GREEN LV_COLOR_MAKE(0x00, 0x80, 0x00)

static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    if (is_initialized_lvgl)
    {
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

static void lv_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
#if defined(TOUCH_READ_FROM_INTERRNUPT)
    if (get_int_signal)
    {
        get_int_signal = false;
        touch.read();
#else
    if (touch.read())
    {
#endif
        TP_Point t = touch.getPoint(0);
        data->point.x = (EXAMPLE_LCD_H_RES - t.x);
        data->point.y = t.y;
        data->state = LV_INDEV_STATE_PR;
    }
    else
        data->state = LV_INDEV_STATE_REL;
}

static void create_UI();
static void create_th_task();
static void create_pic_task();
static void setup_pin();

bool ip_ready = false;
static void connect_wifi()
{
    int cnt = 0;
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Connection Failed!");
        delay(500);
        if (cnt++ > 5)
        {
            Serial.println("Unable to connect to WiFi...");
            goto WIFI_DONE;
        }
    }
    Serial.println("\nConnected to WiFi network with IP Address:");
    Serial.println(WiFi.localIP());
    Serial.print("\nDefault MAC Address: ");
    Serial.println(WiFi.macAddress());
    ip_ready = true;
WIFI_DONE:
    Serial.println("WIFI_DONE");
}

void setup()
{
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    Serial.begin(115200);

    dht.begin(); // initialize the DHT sensor
    dht_room.begin();

    connect_wifi();

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
    };
    esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle);
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_invert_color(panel_handle, true);

    esp_lcd_panel_swap_xy(panel_handle, true);
    esp_lcd_panel_mirror(panel_handle, false, true);
    // the gap is LCD panel specific, even panels with the same driver IC, can
    // have different gap value
    esp_lcd_panel_set_gap(panel_handle, 0, 35);
#if defined(LCD_MODULE_CMD_1)
    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++)
    {
        esp_lcd_panel_io_tx_param(io_handle, lcd_st7789v[i].cmd, lcd_st7789v[i].data, lcd_st7789v[i].len & 0x7f);
        if (lcd_st7789v[i].len & 0x80)
            delay(120);
    }
#endif
    /* Lighten the screen with gradient */
    ledcSetup(0, 10000, 8);
    ledcAttachPin(PIN_LCD_BL, 0);
    for (uint8_t i = 0; i < 0xFF; i++)
    {
        ledcWrite(0, i);
        delay(2);
    }

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

    /* Register touch brush with LVGL */
    Wire.begin(PIN_IIC_SDA, PIN_IIC_SCL, 800000);
    inited_touch = touch.init();
    if (inited_touch)
    {
        touch.setRotation(1);
        static lv_indev_drv_t indev_drv;
        lv_indev_drv_init(&indev_drv);
        indev_drv.type = LV_INDEV_TYPE_POINTER;
        indev_drv.read_cb = lv_touchpad_read;
        lv_indev_drv_register(&indev_drv);
    }
    is_initialized_lvgl = true;
#if defined(TOUCH_READ_FROM_INTERRNUPT)
    attachInterrupt(
        PIN_TOUCH_INT, []
        { get_int_signal = true; },
        FALLING);
#endif

    create_th_task();
    // create_pic_task();
    create_UI();

    setup_pin();

    // #if LV_USE_DEMO_WIDGETS
    //     lv_demo_widgets();
    // #elif LV_USE_DEMO_BENCHMARK
    //     lv_demo_benchmark();
    // #elif LV_USE_DEMO_STRESS
    //     lv_demo_stress();
    // #elif LV_USE_DEMO_KEYPAD_AND_ENCODER
    //     lv_demo_keypad_encoder();
    // #elif LV_USE_DEMO_MUSIC
    //     lv_demo_music();
    // #endif
}

// PID parameters defaults
// double kp=2;   //proportional parameter
// double ki=5;   //integral parameter
// double kd=1;   //derivative parameter

// Setpoint (temperature)
double Setpoint, Input, Output;

// Minimum and Maximum PWM command
// double commandMin = 0;
// double commandMax = 250;

// init PID
PID myPID(&Input, &Output, &Setpoint, 2, 0.5, 0.5, DIRECT);

#define CHAMBER_NAME "chamber1"

float humidity = -99;
float temperature = -99;
float temperature_room = -99;
double t_room_pid = temperature_room;
float humidity_room = -99;
float target_temperature = 25;

float current_input = -99;
float voltage_input = -99;

lv_obj_t *chamber_label;
lv_obj_t *th_label;
lv_obj_t *label_room_t;
lv_obj_t *chamber_status_label;
lv_obj_t *t_target_label;
lv_obj_t *label_enable_t_target;
lv_obj_t *label_current_input;

lv_obj_t *btn_add_t_target;
lv_obj_t *btn_minus_t_target;
lv_obj_t *btn_enable_t_target;

bool enable_t_target = false;

double getTemp()
{

    return temperature;
}

static void setup_pin()
{
    pinMode(rele1, OUTPUT);
    digitalWrite(rele1, LOW);
    pinMode(rele2, OUTPUT);
    digitalWrite(rele2, LOW);

    pinMode(rele_fan, OUTPUT);
    digitalWrite(rele_fan, HIGH);

    pinMode(PWM, OUTPUT);
    analogWrite(PWM, 0);

    Setpoint = target_temperature;
    myPID.SetMode(AUTOMATIC);
}

void lvgl_label_test()
{

    chamber_label = lv_label_create(lv_scr_act());
    String ip_string = (ip_ready) ? WiFi.localIP().toString() : String("0.0.0.0");
    String chamber_str = String(CHAMBER_NAME);
    chamber_str.concat(", ");
    chamber_str.concat(ip_string);
    if (NULL != chamber_label)
    {

        lv_label_set_text(chamber_label, chamber_str.c_str());
        lv_obj_align(chamber_label, LV_ALIGN_TOP_LEFT, 0, 0);
    }

    th_label = lv_label_create(lv_scr_act());
    if (NULL != th_label)
    {
        lv_label_set_text(th_label, "Chamber T / H: ");
        lv_obj_align(th_label, LV_ALIGN_TOP_LEFT, 0, 20);
    }

    label_room_t = lv_label_create(lv_scr_act());
    if (NULL != label_room_t)
    {
        lv_label_set_text(label_room_t, "Room T / H: ");
        lv_obj_align(label_room_t, LV_ALIGN_TOP_LEFT, 0, 40);
    }
    label_current_input = lv_label_create(lv_scr_act());
    if (NULL != label_current_input)
    {
        lv_label_set_text(label_current_input, "Current: ");
        lv_obj_align(label_current_input, LV_ALIGN_TOP_LEFT, 0, 60);
    }

    // t_target_label = lv_label_create(lv_scr_act());
    // if (NULL != t_target_label)
    // {
    //     lv_label_set_text(t_target_label, "Target Temperature: ");
    //     lv_obj_align(t_target_label, LV_ALIGN_TOP_LEFT, 0, 40);
    // }
}

static void update_enable_t_target_label()
{
    lv_label_set_text_fmt(label_enable_t_target, "%.0f", target_temperature);
    if (enable_t_target)
    {
        lv_obj_set_style_bg_color(btn_enable_t_target, LV_COLOR_RED, 0);
    }
    else
    {
        lv_obj_set_style_bg_color(btn_enable_t_target, LV_COLOR_GREEN, 0);
    }
}


unsigned long last_enabled_millis = 0; 

static void btn_event_callback(lv_event_t *event)
{

    lv_obj_t *btn = lv_event_get_target(event);
    if (btn != NULL)
    {
        if (btn == btn_add_t_target && !enable_t_target)
        {
            if (target_temperature < 55)
            {
                target_temperature += 1;
            }
        }

        if (btn == btn_minus_t_target && !enable_t_target)
        {
            if (target_temperature > 0)
            {
                target_temperature -= 1;
            }
        }

        if (btn == btn_enable_t_target)
        {
            if (!enable_t_target)
            {
                unsigned long milli = millis();
                milli = milli - last_enabled_millis;
                if (milli <= 15000)
                {
                    Serial.printf("Unable to enable due to relaxing, wait for %d ms \n", (15000 - milli));
                    return;
                }

                if ((temperature_room >= 0 and temperature_room <= 55) && (target_temperature >= 0 and target_temperature <= 55))
                {
                    t_room_pid = temperature_room;
                    Setpoint = target_temperature;
                }
                else
                {
                    return;
                }
            }
            enable_t_target = !enable_t_target;
        }
        update_enable_t_target_label();
    }
}

void lvgl_button_test()
{

    btn_add_t_target = lv_btn_create(lv_scr_act());
    btn_minus_t_target = lv_btn_create(lv_scr_act());
    btn_enable_t_target = lv_btn_create(lv_scr_act());
    if (btn_enable_t_target != NULL)
    {
        lv_obj_set_size(btn_enable_t_target, 70, 40);
        lv_obj_set_size(btn_add_t_target, 50, 40);
        lv_obj_set_size(btn_minus_t_target, 50, 40);

        lv_obj_add_event_cb(btn_enable_t_target, btn_event_callback, LV_EVENT_CLICKED, NULL);
        lv_obj_add_event_cb(btn_add_t_target, btn_event_callback, LV_EVENT_CLICKED, NULL);
        lv_obj_add_event_cb(btn_minus_t_target, btn_event_callback, LV_EVENT_CLICKED, NULL);
        lv_obj_align(btn_enable_t_target, LV_ALIGN_BOTTOM_LEFT, 0, 0);

        label_enable_t_target = lv_label_create(btn_enable_t_target);
        if (label_enable_t_target != NULL)
        {
            lv_label_set_text(label_enable_t_target, "--"); // 設定顯示內容
            lv_obj_center(label_enable_t_target);           // 物件居中顯示
        }

        lv_obj_t *lbl = lv_label_create(btn_add_t_target);
        lv_label_set_text(lbl, "+");
        lv_obj_center(lbl);
        lbl = lv_label_create(btn_minus_t_target);
        lv_label_set_text(lbl, "-");
        lv_obj_center(lbl);

        lv_obj_align(btn_add_t_target, LV_ALIGN_BOTTOM_LEFT, 85, 0);
        lv_obj_align(btn_minus_t_target, LV_ALIGN_BOTTOM_LEFT, 140, 0);
    }
    update_enable_t_target_label();
}

static void create_UI()
{
    lvgl_label_test();
    lvgl_button_test();
}

bool thTasksEnabled = false;
TaskHandle_t thTaskHandle = NULL;

/** Ticker for temperature reading */
Ticker thTaskTicker;

bool picTasksEnabled = false;
TaskHandle_t picTaskHandle = NULL;
Ticker picTaskTicker;

void pid_control_task(void *pvParameters);

void thTask(void *pvParameters)
{
    Serial.println("tempTask loop started");
    while (1) // tempTask loop
    {
        if (thTasksEnabled)
        { // Read temperature only if old data was processed already
            // Reading temperature for humidity takes about 250 milliseconds!
            // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
            temperature = dht.readTemperature();
            humidity = dht.readHumidity();
            if (isnan(temperature))
            {
                temperature = -99;
                Serial.println("ERROR reading DHT t");
            }

            if (isnan(humidity))
            {
                humidity = -99;
                Serial.println("ERROR reading DHT h");
            }
            

            temperature_room = dht_room.readTemperature();
            humidity_room = dht_room.readHumidity();
            if (isnan(temperature_room))
            {
                temperature_room = -99;
                Serial.println("ERROR reading Room t");
            }

            if (isnan(humidity_room))
            {
                humidity_room = -99;
                Serial.println("ERROR reading Room h");
            }

            int adc = analogRead(CURR_INPUT);
            // Serial.print("adc : ");
            // Serial.println(adc);

            // adc is 3.3V base
            current_input = (((adc - 2075) * 3.3) / 4095.0) / (0.1 * (3.3/5.0));
            if (- 0.2 < current_input && current_input < 0.2) {
                current_input = 0;
            }
            // Serial.print("Current : ");
            // Serial.println(current_input);

            lv_label_set_text_fmt(th_label, "Chamber T / H: %.1f / %.1f", temperature, humidity);
            lv_label_set_text_fmt(label_room_t, "Room T / H: %.1f / %.1f", temperature_room, humidity_room);
            lv_label_set_text_fmt(label_current_input, "Current: %.1f", current_input);
            Serial.printf("Current: %.1f, Voltage: %.1f TC: %.1f, HC: %.1f, TR: %.1f, HR: %.1f\n", current_input, voltage_input, temperature, humidity, temperature_room, humidity_room);
            if (enable_t_target)
            {
                Serial.printf("Setpoint: %.1f, Input: %.1f , Output: %.1f\n", Setpoint, Input, Output);
            }
        }
        vTaskSuspend(NULL);

        
    }
}

/**
 * triggerGetTemp
 * Sets flag dhtUpdated to true for handling in loop()
 * called by Ticker tempTicker
 */
void triggerGetTemp()
{
    if (thTaskHandle != NULL)
    {
        xTaskResumeFromISR(thTaskHandle);
    }
}

void triggerPidControl()
{
    if (picTaskHandle != NULL)
    {
        xTaskResumeFromISR(picTaskHandle);
    }
}

static void create_th_task()
{
    // Start task to get temperature
    xTaskCreatePinnedToCore(
        thTask,        /* Function to implement the task */
        "thTask ",     /* Name of the task */
        4000,          /* Stack size in words */
        NULL,          /* Task input parameter */
        5,             /* Priority of the task */
        &thTaskHandle, /* Task handle. */
        1);            /* Core where the task should run */

    if (thTaskHandle == NULL)
    {
        Serial.println("[ERROR] Failed to start task for temperature update");
    }
    else
    {
        // Start update of environment data every 30 seconds
        thTaskTicker.attach(1, triggerGetTemp);
    }

    // Signal end of setup() to tasks
    thTasksEnabled = true;
}



void pid_control_task(void *pvParameters)
{

    if (!enable_t_target)
    {
        analogWrite(rele1, 0);
        analogWrite(rele2, 0);
        analogWrite(PWM, 0);
        return;
    }

    last_enabled_millis = millis();
    Input = getTemp();
    // process PID

    if (Setpoint >= temperature_room)
    {
        myPID.SetControllerDirection(DIRECT);
        myPID.Compute();
        analogWrite(rele1, 0);
        analogWrite(rele2, Output);
        
    }
    else
    {
        myPID.SetControllerDirection(REVERSE);
        myPID.Compute();
        analogWrite(rele1, Output);
        analogWrite(rele2, 0);
        
    }

    // apply PID processed command
    analogWrite(PWM, Output);
}

static void create_pic_task()
{
    // Start task to get temperature
    xTaskCreatePinnedToCore(
        pid_control_task,        /* Function to implement the task */
        "pid_control_task ",     /* Name of the task */
        4000,          /* Stack size in words */
        NULL,          /* Task input parameter */
        5,             /* Priority of the task */
        &picTaskHandle, /* Task handle. */
        1);            /* Core where the task should run */

    if (picTaskHandle == NULL)
    {
        Serial.println("[ERROR] Failed to start task for PID control");
    }
    else
    {
        // Start update of environment data every 30 seconds
        picTaskTicker.attach(1, triggerPidControl);
    }

    // Signal end of setup() to tasks
    // picTasksEnabled = false;
}

void loop()
{
    lv_timer_handler();
    pid_control_task(NULL);
    // run_pid_control();
    delay(1);
}
