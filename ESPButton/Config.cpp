/*
 * Config.cpp
 *
 *  Created on: Feb 9, 2019
 *      Author: chris.l
 */

#include "Config.h"

#ifndef DBG_CONFIG
#define DBG_CONFIG 1
#endif

#if DBG_CONFIG
#define DBG(fmt, ...) Serial.printf_P( (PGM_P)PSTR(fmt), ## __VA_ARGS__ )
#else
#define DBG(...)
#endif

static const char*    CONFIG_FILENAME    = "/ESPButton.dat";
static const uint32_t CONFIG_MAGIC       = 0xc0c0fefe;

typedef enum value_type : uint8_t
{
    UINT32 = 0,
    STRING = 1
} ValueType;

Config::Config() : _url(), _ntp_server()
{
}

Config::~Config()
{
}

void Config::begin()
{
}

const char* Config::getURL()
{
    return _url.c_str();
}
void Config::setURL(const char* url)
{
    _url = url;
}
const char* Config::getNTPServer()
{
    return _ntp_server.c_str();
}

void Config::setNTPServer(const char* ntp)
{
    _ntp_server = ntp;
}

bool Config::read(File& file, uint32_t& val)
{
    ValueType type;
    if (file.read((uint8_t *)&type, sizeof(type)) != sizeof(type))
    {
        return false;
    }
    if (type != ValueType::UINT32)
    {
        return false;
    }
    if (file.read((uint8_t *)&val, sizeof(val)) != sizeof(val))
    {
        return false;
    }
    return true;
}

bool Config::read(File& file, String& str)
{
    ValueType type;
    if (file.read((uint8_t *)&type, sizeof(type)) != sizeof(type))
    {
        return false;
    }
    if (type != ValueType::STRING)
    {
        return false;
    }
    size_t len;
    if (file.read((uint8_t *)&len, sizeof(len)) != sizeof(len))
    {
        return false;
    }
    uint8_t buf[len+1];
    if (file.read(buf, len) != len)
    {
        return false;
    }
    buf[len] = 0;
    str = (char*)buf;
    return true;
}

bool Config::write(File& file, uint32_t val)
{
    file.write(ValueType::UINT32);
    if (file.write((uint8_t *)&val, sizeof(val)) != sizeof(val))
    {
        return false;
    }
    return true;
}

bool Config::write(File& file, String& str)
{
    size_t len;
    file.write(ValueType::STRING);
    len = str.length();
    if (file.write((uint8_t *)&len, sizeof(len)) != sizeof(len))
    {
        return false;
    }
    if (len > 0 && file.write((uint8_t *)str.c_str(), len) != len)
    {
        return false;
    }
    return true;
}

bool Config::load()
{
    File file = SPIFFS.open(CONFIG_FILENAME, "r");
    if (!file)
    {
        DBG("Config::load: failed to open '%s' for reading!\n", CONFIG_FILENAME);
        return false;
    }

    uint32_t magic;
    if (!read(file, magic) || magic != CONFIG_MAGIC)
    {
        DBG("Config::load: failed to read magic!\n");
        return false;
    }

    if (!read(file, _url))
    {
        DBG("Config::load: failed to read url!\n");
        return false;
    }

    if (!read(file, _ntp_server))
    {
        DBG("Config::load: failed to read ntp_server!\n");
        return false;
    }

    file.close();

    return true;
}

bool Config::save()
{
    File file = SPIFFS.open(CONFIG_FILENAME, "w");
    if (!file)
    {
        DBG("Config::save: failed to open '%s' for writing!\n", CONFIG_FILENAME);
        return false;
    }

    if (!write(file, CONFIG_MAGIC))
    {
        DBG("Config::save: failed to write magic!\n");
        return false;
    }

    if (!write(file, _url))
    {
        DBG("Config::save: failed to write url!\n");
        return false;
    }

    if (!write(file, _ntp_server))
    {
        DBG("Config::save: failed to write ntp_server!\n");
        return false;
    }

    file.close();

    DBG("Config::save success!\n");
    return true;
}
