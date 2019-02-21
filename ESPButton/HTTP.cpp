/*
 * HTTP.cpp
 *
 *  Created on: Feb 10, 2019
 *      Author: chris.l
 */

#include "HTTP.h"
#include <WiFiClientSecure.h>

#ifndef DBG_HTTP
#define DBG_HTTP 1
#endif

#if DBG_HTTP
#define DBG(fmt, ...) Serial.printf_P( (PGM_P)PSTR(fmt), ## __VA_ARGS__ )
#else
#define DBG(...)
#endif

HTTP::HTTP()
{
}

HTTP::~HTTP()
{
}


bool HTTP::GET(const char* url) {
    if (startsWith("https:", url))
    {
        BearSSL::WiFiClientSecure bear;
        // TODO: support certificate CA store or certificate CA from Config
        bear.setInsecure();
        return GET(bear, url);
    }
    WiFiClient client;
    return GET(client, url);
}

bool HTTP::GET(WiFiClient& client, const char* url)
{

    DBG("making HTTPClient\n");
    HTTPClient http;
    http.setUserAgent("HTTPClientTest/1.1");
    //http.setFollowRedirects(true);
    http.setTimeout(10000);
    http.begin(client, url);

    DBG("Starting Request: '%s'\n", url);
    int code = http.GET();
    DBG("http code: %d\n", code);
    DBG("size: %d\n", http.getSize());

    DBG("ending HTTPClient\n");
    http.end();
    return code < 300;
}

bool HTTP::startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}
