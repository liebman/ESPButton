/*
 * RGBLEDSeq.cpp
 *
 *  Created on: Feb 10, 2019
 *      Author: chris.l
 */

#include "RGBSeq.h"

#ifndef DBG_RGBSEQ
#define DBG_RGBSEQ 1
#endif

#ifdef DBG_RGBSEQ
#define DBG(fmt, ...) Serial.printf_P( (PGM_P)PSTR(fmt), ## __VA_ARGS__ )
#else
#define DBG(...)
#endif

RGBSeq::RGBSeq(RGBLED& r, size_t size) : _rgb(r), _items(size)
{
    DBG("RGBLEDSeq constructed with size %u\n", size);
}

RGBSeq::~RGBSeq()
{
    DBG("RGBLEDSeq destructed!\n");
}

void RGBSeq::run(std::initializer_list<RGBSeqItem> l)
{
    _items = l;
    DBG("RGBSeq run with %u items\n", _items.size());
    _item = _items.begin();
    _rgb.set(_item->color);
    _timer.once(_item->duration, std::bind(&RGBSeq::timerAction, this));
}

void RGBSeq::stop()
{
    _timer.detach();
}

void RGBSeq::timerAction()
{
    if (_item != _items.end())
    {
        _item = ++_item;
    }
    else
    {
        _item = _items.begin();
    }
    _rgb.set(_item->color);
    _timer.once(_item->duration, std::bind(&RGBSeq::timerAction, this));
}
