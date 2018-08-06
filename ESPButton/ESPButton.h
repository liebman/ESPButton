
#ifndef _ESPButton_H_
#define _ESPButton_H_

#include "Arduino.h"
#include "ESP8266httpUpdate.h"
#include "WiFiManager.h"
#include "ESP8266HTTPClient.h"
#include "RGBLED.h"
#include "Ticker.h"
#include "Config.h"

#define DEFAULT_NTP_SERVER     "0.zoddotcom.pool.ntp.org"
#define ESP_BUTTON_VERSION "0.3.2"

#define DEBOUNCE_DELAY 20

#define PIN_RED   12
#define PIN_GREEN 13
#define PIN_BLUE  16
#define PIN_ARM   0
#define PIN_TRGR  14

typedef struct color_blink
{
    const RGBColor *on_color;
    float           on_time;
    const RGBColor *off_color;
    float           off_time;
    bool            state;
} ColorBlink;

#endif /* _ESPButton_H_ */
