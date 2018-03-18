
#ifndef _ESPButton_H_
#define _ESPButton_H_

#include "Arduino.h"
#include "ESP8266httpUpdate.h"
#include "WiFiManager.h"
#include "ESP8266HTTPClient.h"
#include "RGBLED.hpp"
#include "Ticker.h"
#include "Config.h"

#define DEFAULT_NTP_SERVER     "0.zoddotcom.pool.ntp.org"
#define ESP_BUTTON_VERSION "0.1.1"

#define DEBOUNCE_DELAY 20

#define PIN_RED   12
#define PIN_GREEN 13
#define PIN_BLUE  16
#define PIN_ARM   0
#define PIN_TRGR  14

typedef struct rgb_color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGBColor;

typedef struct color_blink
{
	const RGBColor *on_color;
	float           on_time;
	const RGBColor *off_color;
	float           off_time;
	bool            state;
} ColorBlink;

const RGBColor black  = {0,0,0};
const RGBColor red    = {255,0,0};
const RGBColor green  = {0,255,0};
const RGBColor blue   = {0,0,255};
const RGBColor yellow = {255,255,0};
const RGBColor orange = {255,127,0};

#endif /* _ESPButton_H_ */
