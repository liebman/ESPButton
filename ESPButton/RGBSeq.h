/*
 * RGBLEDSeq.h
 *
 *  Created on: Feb 10, 2019
 *      Author: chris.l
 */

#ifndef RGBSEQ_H_
#define RGBSEQ_H_

#include "RGBLED.h"
#include <vector>
#include "Ticker.h"
#include <initializer_list>

class RGBSeqItem
{
public:
    RGBSeqItem() : color({0,0,0}), duration(0.0f) {}
    RGBSeqItem(const RGBColor c, float d) : color(c), duration(d) {}
    RGBColor       color;
    float          duration;
};

class RGBSeq
{
public:
    RGBSeq(RGBLED& r, size_t size);
    virtual ~RGBSeq();
    void run(std::initializer_list<RGBSeqItem> l);
    void stop();
protected:
    void timerAction();
private:
    Ticker _timer;
    RGBLED& _rgb;
    std::vector<RGBSeqItem>::iterator _item;
    std::vector<RGBSeqItem> _items;
};

#endif /* RGBSEQ_H_ */
