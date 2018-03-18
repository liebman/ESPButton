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
        dbprintln("Config: formatting SPIFFS...");
        if (SPIFFS.format())
        {
            dbprintln("Config: Done!");
        }
        else
        {
            dbprintln("Config: FAILED!!!!!!");
        }

        if (!SPIFFS.begin())
        {
            dbprintln("Config: SPIFFS.begin() failed!");
        }
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
    dbprintf("Config::setURL: %s\n", url);
    char* old_url = _url;
    size_t len = strlen(url);
    char* new_url = (char*) malloc(len + 1);
    strcpy(new_url, url);
    _url = new_url;
    if (old_url)
    {
        dbprintf("Config::setURL: freeing old url: '%s'\n", old_url);
        free(old_url);
    }
}

const char* Config::getFingerprint()
{
    return _fingerprint != NULL ? _fingerprint : "";
}

void Config::setFingerprint(const char* fingerprint)
{
    dbprintf("Config::setFingerprint: %s\n", fingerprint);
    char* old_fingerprint = _fingerprint;
    size_t len = strlen(fingerprint);
    char* new_fingerprint = (char*) malloc(len + 1);
    strcpy(new_fingerprint, fingerprint);
    _fingerprint = new_fingerprint;
    if (old_fingerprint)
    {
        dbprintf("Config::setFingerprint: freeing old fingerprint: '%s'\n", old_fingerprint);
        free(old_fingerprint);
    }
}

bool Config::save()
{
    dbprintln("Config::save starting!");
    if (!_url)
    {
        dbprintln("Config::save: fail: url not set!");
        return false;
    }

    File file = SPIFFS.open(config_filename, "w");
    if (!file)
    {
        dbprintf("Config::save: failed to open '%s' for writing!\n", config_filename);
        return false;
    }

    dbprintf("Config::save: writing leading magic: 0x%08x\n", config_magic);
    if (file.write((const uint8_t*)&config_magic, sizeof(config_magic)) != sizeof(config_magic))
    {
        dbprintln("Config::save failed to write leading magic!");
        return false;
    }

    //
    // Save url
    //
    if (!writeString(file, _url))
    {
        dbprintln("failed to write URL!");
        return false;
    }

    //
    // Save fingerprint
    //
    if (!writeString(file, _fingerprint))
    {
        dbprintln("failed to write fingerprint!");
        return false;
    }

    dbprintf("Config::save: writing trailing magic: 0x%08x\n", config_magic);
    if (file.write((const uint8_t*)&config_magic, sizeof(config_magic)) != sizeof(config_magic))
    {
        dbprintln("Config::save failed to write trailing magic!");
        return false;
    }

    file.close();
    dbprintln("Config::save success!");
    return true;
}

bool Config::load()
{
    dbprintln("Config::load starting!");

    File file = SPIFFS.open(config_filename, "r");
    if (!file)
    {
        dbprintf("Config::load: failed to open '%s' for reading!\n", config_filename);
        return false;
    }

    uint32_t magic;
    size_t   bytes_read = file.read((uint8_t*)&magic, sizeof(magic));
    if (bytes_read != sizeof(magic))
    {
        dbprintf("Config::load: failed to read leading magic, expected %d got %d bytes!\n", sizeof(magic), bytes_read);
        return false;
    }
    else if (magic != config_magic)
    {
        dbprintf("Config::load: leading magic mismatch! expected 0x%08x got 0x%08x\n", sizeof(magic), bytes_read);
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

    dbprintf("Config::load: new_url: %s\n", new_url);

    //
    // Load fingerprint
    //
    char* new_fingerprint = readString(file);
    if (!new_fingerprint)
    {
        return false;
    }

    dbprintf("Config::load: new_fingerprint: %s\n", new_fingerprint);

    //
    // Trailing magic
    //
    bytes_read = file.read((uint8_t*)&magic, sizeof(magic));
    if (bytes_read != sizeof(magic))
    {
        dbprintf("Config::load: failed to read trailing magic, expected %d got %d bytes!\n", sizeof(magic), bytes_read);
        return false;
    }
    else if (magic != config_magic)
    {
        dbprintf("Config::load: trailing magic mismatch! expected 0x%08x got 0x%08x\n", sizeof(magic), bytes_read);
        return false;
    }

    char* old = _url;
    _url = new_url;
    if (old)
    {
        dbprintf("Config::load freeing old url: '%s'\n", old);
        free(old);
    }

    old = _fingerprint;
    _fingerprint = new_fingerprint;
    if (old)
    {
        dbprintf("Config::load freeing old fingerprint: '%s'\n", old);
        free(old);
    }

    dbprintln("Config::load success!");
    return true;
}

bool Config::writeString(File& file, const char* value)
{
    //
    // write the string id
    //
    if (file.write(config_string_id) != 1)
    {
        dbprintln("Config::writeString failed to write ID!");
        return false;
    }

    //
    // write the string length
    //
    size_t len = strlen(value);
    if (file.write((uint8_t*)&len, sizeof(len)) != sizeof(len))
    {
        dbprintf("Config::writeString failed to write string length (%d)!\n", len);
        return false;
    }

    //
    // write the string itself
    //
    if (file.write((const uint8_t*)value, len) != len)
    {
        dbprintf("Config::writeString failed to write string (%s)!\n", value);
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
        dbprintf("Config::readString: failed to read string id, expected %u got %d\n", config_string_id, id);
        return NULL;
    }

    //
    // read the string length
    //
    size_t len;
    size_t bytes_read = file.read((uint8_t*)&len, sizeof(len));
    if (bytes_read != sizeof(len))
    {
        dbprintf("Config::load: failed to read string length, expected %d got %d bytes!\n", sizeof(len), bytes_read);
        return NULL;
    }

    char* value = (char*)malloc(len+1);
    if (!value)
    {
        dbprintf("Config::load: failed to allocate buffer of %u bytes\n", len);
        return NULL;
    }

    bytes_read = file.read((uint8_t*)value, len);
    if (bytes_read != len)
    {
        dbprintf("Config::load: failed to read host, expected %d got %d bytes!\n", len, bytes_read);
        free(value);
        return NULL;
    }
    value[len] = '\0';

    return value;
}