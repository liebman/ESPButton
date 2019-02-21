/*
 * HTTP.h
 *
 *  Created on: Feb 10, 2019
 *      Author: chris.l
 */

#ifndef HTTP_H_
#define HTTP_H_
#include <ESP8266HTTPClient.h>

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
