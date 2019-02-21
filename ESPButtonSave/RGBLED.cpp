/*
 * RGBLED.cpp
 *
 *  Created on: Jul 4, 2018
 *      Author: chris.l
 */

#include "RGBLED.h"
#include "Logger.h"

#define LED_OFF 1 // common anode
#define PWM_MAX 255

static const char* TAG = "RGBLED";

RGBLED::RGBLED(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin)
: _red_pin(red_pin), _green_pin(green_pin), _blue_pin(blue_pin)
{
    digitalWrite(_red_pin,   LED_OFF);
    digitalWrite(_green_pin, LED_OFF);
    digitalWrite(_blue_pin,  LED_OFF);
    pinMode(_red_pin,   OUTPUT);
    pinMode(_green_pin, OUTPUT);
    pinMode(_blue_pin,  OUTPUT);
    analogWriteRange(PWM_MAX);
}

RGBLED::~RGBLED()
{
}

void RGBLED::set(const RGBColor* color)
{
    dlog.debug(TAG, "::set: red:%d, green:%d, blue:%d", color->red, color->green, color->blue);

    analogWrite(_red_pin,   PWM_MAX - color->red);
    analogWrite(_green_pin, PWM_MAX - color->green);
    analogWrite(_blue_pin,  PWM_MAX - color->blue);
}
