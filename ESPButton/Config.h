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
    bool        save();
    bool        load();

protected:
    bool        read(File& file, uint32_t& val);
    bool        read(File& file, String& str);
    bool        write(File& file, uint32_t val);
    bool        write(File& file, String& str);

private:
    String      _url;
    String      _ntp_server;

};

#endif /* CONFIG_H_ */
