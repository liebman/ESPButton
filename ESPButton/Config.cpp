/*
 * Config.cpp
 *
 *  Created on: Dec 3, 2017
 *      Author: chris.l
 */

#include "Config.h"
#include "Arduino.h"
#include "FS.h"
#include "ArduinoJson.h"
//#define DEBUG
#include "Logger.h"

static const int   JSON_BUFFER_SIZE = 2048;
static const char* CONFIG_FILENAME  = "/ESPButton.json";
static const char* KEY_URL          = "url";
static const char* KEY_NTP_SERVER   = "ntp_server";
static const char *DEFAULT_NTP_SERVER = "0.zoddotcom.pool.ntp.org";

Config::Config() : _url(nullptr), _ntp_server(nullptr)
{
}

Config::~Config()
{
    if (_url)
    {
        free(_url);
    }
}

void Config::begin()
{
    if (!SPIFFS.begin())
    {
        dlog.info("Config", "SPIFFS.begin() failed! (ignoring!)");
    }
}

const char* Config::getURL()
{
    return _url != NULL ? _url : "";
}

void Config::setURL(const char* url)
{
    dlog.info("Config", "::setURL: %s", url);
    char* old_url = _url;
    _url = strdup(url);
    if (old_url)
    {
        dlog.info("Config", "::setURL: freeing old url: '%s'", old_url);
        free(old_url);
    }
}

const char* Config::getNTPServer()
{
    return _ntp_server != NULL ? _ntp_server : DEFAULT_NTP_SERVER;
}

void Config::setNTPServer(const char* ntp)
{
    dlog.info("Config", "::setNtpServer: %s", ntp);
    char* old = _ntp_server;
    _ntp_server = strdup(ntp);
    if (old)
    {
        dlog.info("Config", "::setNTPServer: freeing old ntp_server: '%s'", old);
        free(old);
    }
}

bool Config::save()
{

    dlog.info("Config", "::save starting!");
    if (!_url)
    {
        dlog.info("Config", "::save: fail: url not set!");
        return false;
    }

    File file = SPIFFS.open(CONFIG_FILENAME, "w");
    if (!file)
    {
        dlog.info("Config", "::save: failed to open '%s' for writing!", CONFIG_FILENAME);
        return false;
    }

    DynamicJsonBuffer buffer(JSON_BUFFER_SIZE);
    JsonObject& root = buffer.createObject();

    root[KEY_URL]        = _url;
    root[KEY_NTP_SERVER] = _ntp_server;
    root.printTo(file);

    file.close();

    dlog.info("Config", "::save success!");
    return true;
}

bool Config::load()
{
    dlog.info("Config", "::load starting!");

    File file = SPIFFS.open(CONFIG_FILENAME, "r");
    if (!file)
    {
        dlog.info("Config", "::load: failed to open '%s' for reading!", CONFIG_FILENAME);
        return false;
    }

    DynamicJsonBuffer buffer(JSON_BUFFER_SIZE);
    dlog.debug("Config", "load: parsing file contents");
    JsonObject& root = buffer.parseObject(file);
    file.close();

    if (!root.success())
    {
        dlog.error("Config", "load: fails to parse file!");
        return false;
    }

    if (root.containsKey(KEY_URL))
    {
        //dlog.debug("Config", "load: key: '%s', value: '%s'", KEY_URL, root[KEY_URL]);
        setURL(root[KEY_URL]);
    }

    if (root.containsKey(KEY_NTP_SERVER))
    {
        //dlog.debug("Config", "load: key: '%s', value: '%s'", KEY_NTP_SERVER, root[KEY_NTP_SERVER]);
        setNTPServer(root[KEY_NTP_SERVER]);
    }

    dlog.info("Config", "::load success!");
    return true;
}
