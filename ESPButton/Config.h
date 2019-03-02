/*
 * Config.h
 *
 *  Created on: Feb 9, 2019
 *      Author: chris.l
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#include "Arduino.h"
#include "FS.h"

// enum must use consecutive values!
enum class Phrase
{
    STARTUP = 0,
    ARMED,
    DISARMED,
    ACTIVATED,
    SUCCESS,
    FAILED,
    READY,
    // special values (not really part of enum)
    FIRST = STARTUP,
    LAST = READY,
    SIZE = LAST + 1
};

class Config
{
public:
    Config();
    virtual ~Config();
    void        begin();
    const char* getURL();
    void        setURL(const char* url);
    const char* getNTPServer();
    void        setNTPServer(const char* ntp);
    const char* getSyslog();
    void        setSyslog(const char* host);
    const char* getPhrase(Phrase pid);
    void        setPhrase(Phrase pid, const char* phrase);
    bool        save();
    bool        load();

protected:
    bool        read(File& file, uint32_t& val);
    bool        read(File& file, String& str);
    bool        write(File& file, uint32_t val);
    bool        write(File& file, String& str);

private:
    String _url;
    String _ntp_server;
    String _syslog;
    String _phrase[(int)Phrase::SIZE];
};

#endif /* CONFIG_H_ */
