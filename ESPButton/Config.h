/*
 * Config.h
 *
 *  Created on: Dec 3, 2017
 *      Author: chris.l
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#include "Arduino.h"
#include "FS.h"

class Config {
public:
	Config();
	virtual     ~Config();

	const char* getURL();
	void        setURL(const char* url);
	const char* getFingerprint();
	void        setFingerprint(const char* fingerprint);

	bool        save();
	bool        load();

protected:
	bool   writeString(File& file, const char* value);
	char*  readString(File& file);

private:
	char*    _url;
	char*    _fingerprint;
};

#endif /* CONFIG_H_ */
