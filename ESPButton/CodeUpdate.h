/*
 * Update.h
 *
 *  Created on: Feb 23, 2019
 *      Author: chris.l
 */

#ifndef CODEUPDATE_H_
#define CODEUPDATE_H_
#include "ESP8266httpUpdate.h"

class CodeUpdate
{
public:
    CodeUpdate(uint8_t led);
    virtual ~CodeUpdate();
    bool setUpdate(const char* url);
    bool isUpdate();
    bool update();

protected:
    bool update(const char* url);
    bool update(WiFiClient& client, const char* url);
    bool startsWith(const char *pre, const char *str);
};

#endif /* CODEUPDATE_H_ */
