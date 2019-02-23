/*
 * PolledButton.cpp
 *
 *  Created on: Feb 17, 2019
 *      Author: chris.l
 */

#include "PolledButton.h"

#ifndef DBG_POLLEDBUTTON
#define DBG_POLLEDBUTTON 0
#endif

#if DBG_POLLEDBUTTON
#define DBG(fmt, ...) Serial.printf_P( (PGM_P)PSTR(fmt), ## __VA_ARGS__ )
#else
#define DBG(...)
#endif

PolledButton::PolledButton(uint8_t pin, uint16_t debounce_ms)
    : _activated(0),
      _debounce_ms(debounce_ms),
      _pin(pin)
{
    DBG("PolledButton: pin: %u, debounce: %u\n", _pin, _debounce_ms);
}

PolledButton::~PolledButton()
{
}

bool PolledButton::isPressedNow()
{
    return !digitalRead(_pin);
}

uint32_t PolledButton::isPressed()
{
    // if button not pressed, reset activation
    if (digitalRead(_pin) && _activated != 0)
    {
        DBG("PolledButton: pin: %u, deactivating\n", _pin);
        _activated = 0;
        return 0;
    }

    // button is pressed, set activation timestamp
    if (_activated == 0 && !digitalRead(_pin))
    {
        _activated = millis();
        DBG("PolledButton: pin: %u, activating %u\n", _pin, _activated);
        return 0;
    }

    // if debounce time has not passed
    if (_activated > millis()+_debounce_ms)
    {
        return 0;
    }

    return _activated;
}
