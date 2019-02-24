/*
 * Update.cpp
 *
 *  Created on: Feb 23, 2019
 *      Author: chris.l
 */

#include "CodeUpdate.h"

#include "FS.h"

#ifndef DBG_UPDATE
#define DBG_UPDATE 1
#endif

#if DBG_UPDATE
#define DBG(fmt, ...) Serial.printf_P( (PGM_P)PSTR(fmt), ## __VA_ARGS__ )
#else
#define DBG(...)
#endif

static const char*    UPDATE_FILENAME    = "/ESPButton.upd";

CodeUpdate::CodeUpdate(uint8_t led)
{
    ESPhttpUpdate.setLedPin(led, LOW);
}

CodeUpdate::~CodeUpdate()
{
}

bool CodeUpdate::setUpdate(const char* url)
{
    size_t len = strlen(url);
    if (len < 1)
    {
        return true;
    }
    DBG("Setting update url to '%s'\n", url);
    File file = SPIFFS.open(UPDATE_FILENAME, "w");
    if (!file)
    {
        DBG("Update::setUpdate: failed to open '%s' for writing!\n", UPDATE_FILENAME);
        return false;
    }
    if (file.write((uint8_t *)url, len) != len)
    {
        DBG("Update::setUpdate: failed to write url to '%s'\n", UPDATE_FILENAME);
        file.close();
        SPIFFS.remove(UPDATE_FILENAME);
        return false;
    }
    file.close();
    DBG("Update::setUpdate: update url set to '%s'\n", url);
    return true;
}

bool CodeUpdate::isUpdate()
{
    return SPIFFS.exists(UPDATE_FILENAME);
}

bool CodeUpdate::update()
{
    File file = SPIFFS.open(UPDATE_FILENAME, "r");
    if (!file)
    {
        DBG("Update::update: failed to open '%s' for reading!\n", UPDATE_FILENAME);
        return false;
    }
    size_t len = file.size();
    char* url = (char*)malloc(len+1);
    if (url == nullptr)
    {
        DBG("Update::update: failed to allocate %u bytes for url\n", len+1);
    }
    bzero(url, len+1);
    if (file.read((uint8_t *)url, len) != len)
    {
        DBG("Update::update: failed to read url!\n");
        file.close();
        SPIFFS.remove(UPDATE_FILENAME);
        free(url);
        return false;
    }

    file.close();
    SPIFFS.remove(UPDATE_FILENAME);

    bool ret = update(url);
    free(url);
    return ret;
}

bool CodeUpdate::update(WiFiClient& client, const char* url) {

    DBG("CodeUpdate::update: starting update!\n");
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, url);

    switch (ret)
    {
        case HTTP_UPDATE_FAILED:
            DBG("HTTP_UPDATE_FAILD Error (%d): %s\n",
                    ESPhttpUpdate.getLastError(),
                    ESPhttpUpdate.getLastErrorString().c_str());
            return false;

        case HTTP_UPDATE_NO_UPDATES:
            DBG("HTTP_UPDATE_NO_UPDATES\n");
            break;

        case HTTP_UPDATE_OK:
            DBG("HTTP_UPDATE_OK\n");
            break;
    }

    return true;
}

bool CodeUpdate::update(const char* url) {
    if (startsWith("https:", url))
    {
        BearSSL::WiFiClientSecure bear;
        // TODO: support certificate CA store or certificate CA from Config
        bear.setInsecure();
        return update(bear, url);
    }
    WiFiClient client;
    return update(client, url);
}

bool CodeUpdate::startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}
