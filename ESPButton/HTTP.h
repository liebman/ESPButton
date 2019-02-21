/*
 * HTTP.h
 *
 *  Created on: Feb 10, 2019
 *      Author: chris.l
 */

#ifndef HTTP_H_
#define HTTP_H_
#include <ESP8266HTTPClient.h>

#ifndef DBG_HTTP
#define DBG_HTTP 1
#endif

#ifdef DBG_HTTP
#define DBG(fmt, ...) Serial.printf_P( (PGM_P)PSTR(fmt), ## __VA_ARGS__ )
#else
#define DBG(...)
#endif

class HTTP: public HTTPClient
{
public:
    HTTP();
    virtual ~HTTP();
    bool GET(const char* url);
protected:
    bool startsWith(const char* pre, const char* str);
    bool GET(WiFiClient& client, const char* url);
};

#endif /* HTTP_H_ */
