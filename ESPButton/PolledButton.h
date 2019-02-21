/*
 * PolledButton.h
 *
 *  Created on: Feb 17, 2019
 *      Author: chris.l
 */

#ifndef POLLEDBUTTON_H_
#define POLLEDBUTTON_H_
#include "Arduino.h"


#define PB_DEFAULT_DEBOUNCE_MS 20 // 20ms by default

class PolledButton
{
public:
    PolledButton(uint8_t pin, uint16_t debuonce_ms = PB_DEFAULT_DEBOUNCE_MS);
    virtual ~PolledButton();
    uint32_t isPressed(); // returns the pressed timestamp if pressed
private:
    uint32_t _activated;
    uint16_t _debounce_ms;
    uint8_t _pin;
};

#endif /* POLLEDBUTTON_H_ */
