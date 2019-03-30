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
static const uint16_t CONFIG_VERSION     = 0x0001;

enum class ValueType : uint8_t
{
    UINT32 = 0,
    STRING = 1
};

Config::Config() :
        _url(),
        _ntp_server(),
        _syslog(),
        _phrase()
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

const char* Config::getSyslog()
{
    return _syslog.c_str();
}

void Config::setSyslog(const char* syslog)
{
    _syslog = syslog;
}

const char* Config::getPhrase(Phrase pid)
{
    if (pid > Phrase::LAST)
    {
        DBG("Config::getPhrase: invalid phrase identifier: %d\n", pid);
        return "invalid phrase identifier!";
    }
    return _phrase[(int)pid].c_str();
}

void Config::setPhrase(Phrase pid, const char* phrase)
{
    if (pid > Phrase::LAST)
    {
        DBG("Config::setPhrase: invalid phrase identifier: %d '%s'\n", pid, phrase);
        return;
    }
    _phrase[(int)pid] = phrase;
}

const char* Config::getSSID()
{
    return _ssid.c_str();
}

void Config::setSSID(const char* ssid)
{
    _ssid = ssid;
}

const char* Config::getPSK()
{
    return _psk.c_str();
}

void Config::setPSK(const char* psk)
{
    _psk = psk;
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
    file.write((uint8_t)ValueType::UINT32);
    if (file.write((uint8_t *)&val, sizeof(val)) != sizeof(val))
    {
        return false;
    }
    return true;
}

bool Config::write(File& file, String& str)
{
    size_t len;
    file.write((uint8_t)ValueType::STRING);
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
    DBG("Config::load: magic: 0x%08x\n", magic);

    uint32_t version;
    if (!read(file, version) || version != CONFIG_VERSION)
    {
        DBG("Config::load: failed to read version!\n");
        return false;
    }
    DBG("Config::load: version: 0x%04x\n", version);

    if (!read(file, _url))
    {
        DBG("Config::load: failed to read url!\n");
        return false;
    }
    DBG("Config::load: URL: '%s'\n", _url.c_str());

    if (!read(file, _ntp_server))
    {
        DBG("Config::load: failed to read ntp_server!\n");
        return false;
    }
    DBG("Config::load: NTP: '%s'\n", _ntp_server.c_str());

    if (!read(file, _syslog))
    {
        DBG("Config::load: failed to read syslog!\n");
        return false;
    }
    DBG("Config::load: syslog: '%s'\n", _syslog.c_str());

    for (int pid = (int)Phrase::FIRST; pid < (int)Phrase::SIZE; pid++)
    {
        if (!read(file, _phrase[pid]))
        {
            DBG("Config::load: failed to read phrase %d!\n", pid);
            return false;
        }
        DBG("Config::load: phrase[%d]: '%s'\n",pid,  _phrase[pid].c_str());
    }

    if (!read(file, _ssid))
    {
        DBG("Config::load: failed to read SSID!\n");
        return false;
    }
    DBG("Config::load: SSID: '%s'\n", _ssid.c_str());

    if (!read(file, _psk))
    {
        DBG("Config::load: failed to read PSK!\n");
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
    DBG("Config::save: magic: 0x%08x\n", CONFIG_MAGIC);

    if (!write(file, CONFIG_VERSION))
    {
        DBG("Config::save: failed to write version!\n");
        return false;
    }
    DBG("Config::save: version: 0x%04x\n", CONFIG_VERSION);

    if (!write(file, _url))
    {
        DBG("Config::save: failed to write url!\n");
        return false;
    }
    DBG("Config::save: URL: '%s'\n", _url.c_str());

    if (!write(file, _ntp_server))
    {
        DBG("Config::save: failed to write ntp_server!\n");
        return false;
    }
    DBG("Config::save: NTP: '%s'\n", _ntp_server.c_str());

    if (!write(file, _syslog))
    {
        DBG("Config::save: failed to write syslog!\n");
        return false;
    }
    DBG("Config::save: syslog: '%s'\n", _syslog.c_str());

    for (int pid = (int)Phrase::FIRST; pid < (int)Phrase::SIZE; pid++)
    {
        if (!write(file, _phrase[pid]))
        {
            DBG("Config::load: failed to write phrase %d!\n", pid);
            return false;
        }
        DBG("Config::save: phrase[%d]: '%s'\n",pid,  _phrase[pid].c_str());
    }

    if (!write(file, _ssid))
    {
        DBG("Config::save: failed to write ssid!\n");
        return false;
    }
    DBG("Config::save: SSID: '%s'\n", _ssid.c_str());

    if (!write(file, _psk))
    {
        DBG("Config::save: failed to write PSK!\n");
        return false;
    }

    file.close();

    DBG("Config::save success!\n");
    return true;
}
