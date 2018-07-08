/*
 * Config.cpp
 *
 *  Created on: Dec 3, 2017
 *      Author: chris.l
 */

#include "Config.h"
//#define DEBUG
#include "Logger.h"


const uint32_t config_magic     = 0xfadefeed;
const uint8_t  config_version   = 0;
const char*    config_filename  = "/ESPButton.dat";
const uint8_t  config_string_id = 1;

Config::Config()
{
    _url         = NULL;
    _fingerprint = NULL;

    if (!SPIFFS.begin())
    {
        dlog.info("Config", "SPIFFS.begin() failed! (ignoring!)");
    }
}

Config::~Config()
{
    if (_url)
    {
        free(_url);
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
    size_t len = strlen(url);
    char* new_url = (char*) malloc(len + 1);
    strcpy(new_url, url);
    _url = new_url;
    if (old_url)
    {
        dlog.info("Config", "::setURL: freeing old url: '%s'", old_url);
        free(old_url);
    }
}

const char* Config::getFingerprint()
{
    return _fingerprint != NULL ? _fingerprint : "";
}

void Config::setFingerprint(const char* fingerprint)
{
    dlog.info("Config", "::setFingerprint: %s", fingerprint);
    char* old_fingerprint = _fingerprint;
    size_t len = strlen(fingerprint);
    char* new_fingerprint = (char*) malloc(len + 1);
    strcpy(new_fingerprint, fingerprint);
    _fingerprint = new_fingerprint;
    if (old_fingerprint)
    {
        dlog.info("Config", "::setFingerprint: freeing old fingerprint: '%s'", old_fingerprint);
        free(old_fingerprint);
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

    File file = SPIFFS.open(config_filename, "w");
    if (!file)
    {
        dlog.info("Config", "::save: failed to open '%s' for writing!", config_filename);
        return false;
    }

    dlog.info("Config", "::save: writing leading magic: 0x%08x", config_magic);
    if (file.write((const uint8_t*)&config_magic, sizeof(config_magic)) != sizeof(config_magic))
    {
        dlog.info("Config", "::save failed to write leading magic!");
        return false;
    }

    //
    // Save url
    //
    if (!writeString(file, _url))
    {
        dlog.info("Config", "::save: failed to write URL!");
        return false;
    }

    //
    // Save fingerprint
    //
    if (!writeString(file, _fingerprint))
    {
        dlog.info("Config", "::save: failed to write fingerprint!");
        return false;
    }

    dlog.info("Config", "::save: writing trailing magic: 0x%08x", config_magic);
    if (file.write((const uint8_t*)&config_magic, sizeof(config_magic)) != sizeof(config_magic))
    {
        dlog.info("Config", "::save failed to write trailing magic!");
        return false;
    }

    file.close();
    dlog.info("Config", "::save success!");
    return true;
}

bool Config::load()
{
    dlog.info("Config", "::load starting!");

    File file = SPIFFS.open(config_filename, "r");
    if (!file)
    {
        dlog.info("Config", "::load: failed to open '%s' for reading!", config_filename);
        return false;
    }

    uint32_t magic;
    size_t   bytes_read = file.read((uint8_t*)&magic, sizeof(magic));
    if (bytes_read != sizeof(magic))
    {
        dlog.info("Config", "::load: failed to read leading magic, expected %d got %d bytes!", sizeof(magic), bytes_read);
        return false;
    }
    else if (magic != config_magic)
    {
        dlog.info("Config", "::load: leading magic mismatch! expected 0x%08x got 0x%08x", sizeof(magic), bytes_read);
        return false;
    }

    //
    // Load URL
    //
    char* new_url = readString(file);
    if (!new_url)
    {
        return false;
    }

    dlog.info("Config", "::load: new_url: %s", new_url);

    //
    // Load fingerprint
    //
    char* new_fingerprint = readString(file);
    if (!new_fingerprint)
    {
        return false;
    }

    dlog.info("Config", "::load: new_fingerprint: %s", new_fingerprint);

    //
    // Trailing magic
    //
    bytes_read = file.read((uint8_t*)&magic, sizeof(magic));
    if (bytes_read != sizeof(magic))
    {
        dlog.info("Config", "::load: failed to read trailing magic, expected %d got %d bytes!", sizeof(magic), bytes_read);
        return false;
    }
    else if (magic != config_magic)
    {
        dlog.info("Config", "::load: trailing magic mismatch! expected 0x%08x got 0x%08x", sizeof(magic), bytes_read);
        return false;
    }

    char* old = _url;
    _url = new_url;
    if (old)
    {
        dlog.info("Config", "::load freeing old url: '%s'", old);
        free(old);
    }

    old = _fingerprint;
    _fingerprint = new_fingerprint;
    if (old)
    {
        dlog.info("Config", "::load freeing old fingerprint: '%s'", old);
        free(old);
    }

    dlog.info("Config", "::load success!");
    return true;
}

bool Config::writeString(File& file, const char* value)
{
    //
    // write the string id
    //
    if (file.write(config_string_id) != 1)
    {
        dlog.info("Config", "::writeString failed to write ID!");
        return false;
    }

    //
    // write the string length
    //
    size_t len = strlen(value);
    if (file.write((uint8_t*)&len, sizeof(len)) != sizeof(len))
    {
        dlog.info("Config", "::writeString failed to write string length (%d)!", len);
        return false;
    }

    //
    // write the string itself
    //
    if (file.write((const uint8_t*)value, len) != len)
    {
        dlog.info("Config", "::writeString failed to write string (%s)!", value);
        return false;
    }

    return true;
}

char* Config::readString(File& file)
{
    //
    // read & validate the string ID
    //
    int id = file.read();
    if (id != config_string_id)
    {
        dlog.info("Config", "::readString: failed to read string id, expected %u got %d", config_string_id, id);
        return NULL;
    }

    //
    // read the string length
    //
    size_t len;
    size_t bytes_read = file.read((uint8_t*)&len, sizeof(len));
    if (bytes_read != sizeof(len))
    {
        dlog.info("Config", "::load: failed to read string length, expected %d got %d bytes!", sizeof(len), bytes_read);
        return NULL;
    }

    char* value = (char*)malloc(len+1);
    if (!value)
    {
        dlog.info("Config", "::load: failed to allocate buffer of %u bytes", len);
        return NULL;
    }

    bytes_read = file.read((uint8_t*)value, len);
    if (bytes_read != len)
    {
        dlog.info("Config", "::load: failed to read host, expected %d got %d bytes!", len, bytes_read);
        free(value);
        return NULL;
    }
    value[len] = '\0';

    return value;
}
