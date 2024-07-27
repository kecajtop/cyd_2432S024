#include "Arduino.h"
#include <EEPROM.h>
#include <rom/rtc.h>
#include "I2C_eeprom.h"
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
#include "sd_fw_upgrade.h"
#include <XPT2046_Touchscreen.h>
#include "sd_config.h"
#include "sd_card_csv.h"
#include "ee_config.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include "ESP32OTAPull.h"

#if __has_include("update_settings.h") // optionally override with values in settings.h
#include "update_settings.h"
#else
#define JSON_URL   "https://raw.githubusercontent.com/kecajtop/cyd_2432S024/main/json/cyd_2432S024.json" // this is where you'll post your JSON filter file
#define SSID 	   "<my WiFi SSID>"
#define PASS       "<my WiFi Password>"
#define VERSION    "1.0.0" // The current version of this program
#endif


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

SimpleSyslog syslog("Tester", "Tester01", "192.168.2.2");
	// Note:
	// FAC_USER, and FAC_LOCAL0 through FAC_LOCAL7 are valid facilities
	// PRI_EMERGENCY, PRI_ALERT, PRI_CRITICAL, PRI_ERROR, PRI_WARNING, PRI_NOTICE, PRI_INFO, and PRI_DEBUG are valid

//#define F_CPU 8000000L // CPU clock speed 16 MHz

#define I2C_CLKRATE_400K            400000      // Speed of I2C bus 100KHz

//I2C SDA 21
//I2C SCL 22

//I2C UART0 TX 1
//I2C UART0 RX 3

#define INT 19

#define USE_LV_LOG 1

//SPIClass sdSPI(HSPI);

//I2C_eeprom ee(0x50);

char data_block[128]; 


pattern_struct_t pattern;

//syslog.printf(FAC_USER, PRI_WARNING, "Looping around... Uptime: %0.1f minutes", millis() / (float)60000);


void callback_dots(int offset, int totallength);
void callback_percent(int offset, int totallength);
const char *errtext(int code);


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
    Serial.print("X = ");
    Serial.print(x);
    Serial.print(" | Y = ");
    Serial.print(y);
    Serial.print(" | Pressure = ");
    Serial.print(z);
    Serial.println();
  }
  else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

// Callback that is triggered when btn1 is clicked
static void event_handler_btn1(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  if(code == LV_EVENT_CLICKED) {
    LV_LOG_USER("Load Button clicked ");
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
  lv_label_set_text(btn_label, "LOAD");
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

  lv_example_roller_1();
}

void setup() {
  // put your setup code here, to run once:
  
    Serial.begin(115200);
    Serial.println("");

    lv_init();
    static uint32_t prev_ms = millis();

    lv_log_register_print_cb(log_print);

      // Start the SPI for the touchscreen and init the touchscreen
    touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touchscreen.begin(touchscreenSPI);
    // Set the Touchscreen rotation in landscape mode
    // Note: in some displays, the touchscreen might be upside down, so you might need to set the rotation to 1: touchscreen.setRotation(1);
    touchscreen.setRotation(3);

    init_tft();

    clear_display();
    
    set_selftest_tite("INFO");
    
    app_info();
    
    lib_info();

    clear_display();

    set_selftest_tite("SELFTEST"); 

    //convert2pattern();
    //set_pin_pattern(find_pin(2,4),0);
    //set_pin_pattern(find_pin(1,6),0);
    //set_pin_pattern(find_pin(1,5),0);
    //set_pin_pattern(find_pin(2,3),0);
    
    build_date();
    
    reset_info();

    stauts = sd_init();

    display_selftest_pass_fail("SD CARD",stauts);

    stauts = sd_new_fw();

    //load_csv("12345");

    //clear_tables();
    
    
    //delay(5000);

    //ESP.restart();

    //while(1);

    display_selftest_pass_fail("NEW FIRMWARE",stauts);

    sd_fw_upgrade();

    stauts = load_config();

    display_selftest_pass_fail("config.txt",stauts);

    if (settings.enable_wifi)
    {
      Serial.printf("[M] Enabling WiFi \n\r");
      if (initWiFi("Tester"))
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

    ESP32OTAPull ota;

    	// Example 1: See if an update is available (but don't do it).  No callback routine
	Serial.printf("Check for update, but don't download it.\n");
  Serial.printf(JSON_URL);
	int ret = ota
		.CheckForOTAUpdate(JSON_URL, VERSION, ESP32OTAPull::DONT_DO_UPDATE);
	Serial.printf("CheckForOTAUpdate returned %d (%s)\n\n", ret, errtext(ret));

	// After we've checked, we can obtain the version from the JSON file
    String otaVersion = ota.GetVersion();
	Serial.printf("OTA Version Available: %s\n", otaVersion.c_str());

	delay(3000);

	// Example 2
	Serial.printf("Check for update and download it, but don't reboot.  Display dots.\n");
	ret = ota
		.SetCallback(callback_dots)
		.CheckForOTAUpdate(JSON_URL, VERSION, ESP32OTAPull::UPDATE_BUT_NO_BOOT);
	Serial.printf("CheckForOTAUpdate returned %d (%s)\n\n", ret, errtext(ret));

	delay(3000);

	// Example 3
	Serial.printf("Download and install downgrade, but only if the configuration string matches.  Display percentages.\n");
	ret = ota
		.SetCallback(callback_percent)
		.AllowDowngrades(true)
		.SetConfig("4MB RAM")
		.CheckForOTAUpdate(JSON_URL, "99.99.99", ESP32OTAPull::UPDATE_AND_BOOT);
	Serial.printf("CheckForOTAUpdate returned %d (%s)\n\n", ret, errtext(ret));
    
    Wire.begin();

    Wire.setClock(I2C_CLKRATE_400K);

    //ee_init();

    if (!ee_info())
    {
    //  gpio_error();
      display_selftest_pass_fail("EEPROM",ST_NOK);
    //  _mode = _FAIL_MODE;
    }
    else
    {
      display_selftest_pass_fail("EEPROM",ST_OK);
    }

    if (settings.start)
    {
      Serial.printf("[M] Enabling Auto load \n\r");
      display_selftest_pass_fail("AUTO LOAD",ST_ENABLED);
    }
    else
    {
      display_selftest_pass_fail("AUTO LOAD",ST_DISABLED);
    }


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
    
    while(!settings.start)
    {
    
    lv_task_handler(); /* Handle LVGL tasks */


    uint32_t elapsed_ms = millis() - prev_ms;

    if(elapsed_ms >= LVGL_TICK_PERIOD_MS) {
        /* Increment LVGL's internal tick count */
        lv_tick_inc(elapsed_ms);

        /* Update record of when we last updated LVGL's tick count */
        prev_ms += elapsed_ms;
    }          // let this time pass
    }     

    UsrLogln("Ready!");
}

void loop()
{
  
}

const char *errtext(int code)
{
	switch(code)
	{
		case ESP32OTAPull::UPDATE_AVAILABLE:
			return "An update is available but wasn't installed";
		case ESP32OTAPull::NO_UPDATE_PROFILE_FOUND:
			return "No profile matches";
		case ESP32OTAPull::NO_UPDATE_AVAILABLE:
			return "Profile matched, but update not applicable";
		case ESP32OTAPull::UPDATE_OK:
			return "An update was done, but no reboot";
		case ESP32OTAPull::HTTP_FAILED:
			return "HTTP GET failure";
		case ESP32OTAPull::WRITE_ERROR:
			return "Write error";
		case ESP32OTAPull::JSON_PROBLEM:
			return "Invalid JSON";
		case ESP32OTAPull::OTA_UPDATE_FAIL:
			return "Update fail (no OTA partition?)";
		default:
			if (code > 0)
				return "Unexpected HTTP response code";
			break;
	}
	return "Unknown error";
}


void callback_percent(int offset, int totallength)
{
	static int prev_percent = -1;
	int percent = 100 * offset / totallength;
	if (percent != prev_percent)
	{
		Serial.printf("Updating %d of %d (%02d%%)...\n", offset, totallength, 100 * offset / totallength);
    display_firmware_update_progress(offset, totallength);
		prev_percent = percent;
	}
}

void callback_dots(int offset, int totallength)
{
	Serial.print(".");
}