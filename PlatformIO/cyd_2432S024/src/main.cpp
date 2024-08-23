#include "Arduino.h"
#include "Wire.h"
#include <EEPROM.h>
#include <rom/rtc.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <SimpleSyslog.h>
#include "network.h"
#include "config.h"
#include "info.h"
#include "sd_card.h"
#include "lvgl.h"
#include "demos/lv_demos.h"
#include "macros.h"
#include "simple_tft.h"
#include "fw_upgrade.h"
#include <XPT2046_Touchscreen.h>
#include "ams.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip


TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define LVGL_TICK_PERIOD_MS 1

#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

// Touchscreen pins
//#define XPT2046_IRQ 36   // T_IRQ
//#define XPT2046_MOSI 13  // T_DIN
//#define XPT2046_MISO 12  // T_OUT
//#define XPT2046_CLK 14   // T_CLK
//#define XPT2046_CS 33    // T_CS

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

int x, y, z;

// Replace with your network credentials

const char* ssid; //= "";
const char* password; //= "";

const char* test_file_name = "test";

char list[512];

int stauts = 0;

config_t settings;

static uint32_t prev_ms = 0;

SimpleSyslog syslog("Tester", "Tester01", "192.168.2.2");

#define I2C_CLKRATE_400K            400000      // Speed of I2C bus 100KHz

//I2C SDA 21
//I2C SCL 22

//I2C UART0 TX 1
//I2C UART0 RX 3

#define INT 19

#define USE_LV_LOG 1

char data_block[128]; 

pattern_struct_t pattern;

//syslog.printf(FAC_USER, PRI_WARNING, "Looping around... Uptime: %0.1f minutes", millis() / (float)60000);

//#if LV_USE_LOG != 0
void my_print( lv_log_level_t level, const char * buf )
{
    LV_UNUSED(level);
    Serial.println(buf);
    Serial.flush();
}
//#endif

static uint32_t my_tick(void)
{
    return millis();
}

void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data) {
  // Checks if Touchscreen was touched, and prints X, Y and Pressure (Z)
  if(touchscreen.tirqTouched() && touchscreen.touched()) {
    // Get Touchscreen points
    TS_Point p = touchscreen.getPoint();
    // Calibrate Touchscreen points with map function to the correct width and height
    x = map(p.y, 200, 3700, 1, SCREEN_WIDTH);
    y = map(p.x, 240, 3800, 1, SCREEN_HEIGHT);
    z = p.z;

    data->state = LV_INDEV_STATE_PRESSED;

    // Set the coordinates
    data->point.x = x;
    data->point.y = y;

    // Print Touchscreen info about X, Y and Pressure (Z) on the Serial Monitor
    print_k("X = ");
    print_k(x);
    print_k(" | Y = ");
    print_k(y);
    print_k(" | Pressure = ");
    print_k(z);
    print_kln();
  }
  else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

// Callback that is triggered when btn1 is clicked
static void event_handler_btn1(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  if(code == LV_EVENT_CLICKED) {
    LV_LOG_USER("LED Button clicked ");
    led_toggle();
  }
}

// Callback that is triggered when btn2 is clicked
static void event_handler_btn2(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  if(code == LV_EVENT_CLICKED) {
    //settings.start = 1;
    LV_LOG_USER("Start Button clicked ");
  }
}

// Callback that is triggered when btn2 is clicked
static void event_handler_btn3(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  if(code == LV_EVENT_CLICKED) {
    
    LV_LOG_USER("Restart Button clicked ");
    ESP.restart();
  }
}


static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = (lv_obj_t*) lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_roller_get_selected_str(obj, buf, sizeof(buf));
        LV_LOG_USER("Selected file: %s\n", buf);
        //strcpy(test_file_name, buf);
    }
}

/**
 * An infinite roller with the name of the months
 */
void lv_example_roller_1(void)
{
    lv_obj_t * roller1 = lv_roller_create(lv_screen_active());
    lv_roller_set_options(roller1,list, LV_ROLLER_MODE_INFINITE);

    lv_roller_set_visible_row_count(roller1, 4);
    lv_obj_set_style_text_align(roller1, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_center(roller1);
    lv_obj_add_event_cb(roller1, event_handler, LV_EVENT_ALL, NULL);
}

void lv_create_main_gui(void) 
{
  lv_obj_t * btn_label;
  // Create a Button (btn1)
  lv_obj_t * btn1 = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn1, event_handler_btn1, LV_EVENT_ALL, NULL);
  lv_obj_align(btn1, LV_ALIGN_CENTER, 100, 0);
  lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);

  btn_label = lv_label_create(btn1);
  lv_label_set_text(btn_label, "LED");
  lv_obj_center(btn_label);

  lv_obj_t * btn2 = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn2, event_handler_btn2, LV_EVENT_ALL, NULL);
  lv_obj_align(btn2, LV_ALIGN_CENTER, 100, -100);
  lv_obj_remove_flag(btn2, LV_OBJ_FLAG_PRESS_LOCK);

  btn_label = lv_label_create(btn2);
  lv_label_set_text(btn_label, "START");
  lv_obj_center(btn_label);

  lv_obj_t * btn3 = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn3, event_handler_btn3, LV_EVENT_ALL, NULL);
  lv_obj_align(btn3, LV_ALIGN_CENTER, 100, 100);
  lv_obj_remove_flag(btn3, LV_OBJ_FLAG_PRESS_LOCK);

  btn_label = lv_label_create(btn3);
  lv_label_set_text(btn_label, "REBOOT");
  lv_obj_center(btn_label);

  //lv_example_roller_1();
}

void setup() {
  // put your setup code here, to run once:
  
    Serial.begin(115200);
    infoln("Start");

    lv_init();
    
    prev_ms = millis();

    lv_log_register_print_cb(log_print);

    init_tft();

    touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touchscreen.begin(touchscreenSPI);
    // Set the Touchscreen rotation in landscape mode
    // Note: in some displays, the touchscreen might be upside down, so you might need to set the rotation to 1: touchscreen.setRotation(1);
    touchscreen.setRotation(3);

    clear_display();
    
    set_selftest_tite("INFO");
    
    app_info();
    
    lib_info();

    clear_display();

    set_selftest_tite("SELFTEST"); 

    build_date();
    
    reset_info();

    stauts = sd_init();

    display_selftest_pass_fail("SD CARD",stauts);
    
    if (stauts)
    {
      stauts = sd_new_fw();
      if (stauts)
      {
        display_selftest_pass_fail("SD CARD FW.",stauts);
        sd_fw_upgrade();
      }
    }

    stauts = load_config();

    display_selftest_pass_fail("CONFIG TXT",stauts);

    if (settings.enable_wifi)
    {
      print_kln("[M] Enabling WiFi");
      if (initWiFi("Test"))
      {
        display_selftest_pass_fail("WIFI",ST_CONNECTED);
        
      }
      else
      {
        display_selftest_pass_fail("WIFI",ST_DISCONNECTED);
      }
    }
    else
    {
      display_selftest_pass_fail("WIFI",ST_DISABLED);
    }
    
    Wire.begin();

    Wire.setClock(I2C_CLKRATE_400K);

    init_ams();

    delay(2000);

    tft.setRotation(3);

    lv_display_t * disp;

    disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    // Set the callback function to read Touchscreen input
    lv_indev_set_read_cb(indev, touchscreen_read);

    // Function to draw the GUI (text, buttons and sliders)

    lv_create_main_gui();

    //ams.drvOn();

    infoln("Ready!");
}



void loop()
{
    lv_task_handler(); /* Handle LVGL tasks */

    //show_graph();

    show_colour();

    uint32_t elapsed_ms = millis() - prev_ms;

    if(elapsed_ms >= LVGL_TICK_PERIOD_MS) {
        /* Increment LVGL's internal tick count */
        lv_tick_inc(elapsed_ms);

        /* Update record of when we last updated LVGL's tick count */
        prev_ms += elapsed_ms;
    }          // let this time pass
  
}

