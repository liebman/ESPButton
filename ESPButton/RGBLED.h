/*
 * RGBLED.h
 *
 *  Created on: Jul 4, 2018
 *      Author: chris.l
 */

#ifndef RGBLED_H_
#define RGBLED_H_

#include "Arduino.h"

typedef struct rgb_color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGBColor;

class RGBLED
{
public:
    RGBLED(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin);
    virtual ~RGBLED();
    void set(const RGBColor& color);
private:
    uint8_t _red_pin;
    uint8_t _green_pin;
    uint8_t _blue_pin;
};

#endif /* RGBLED_H_ */
