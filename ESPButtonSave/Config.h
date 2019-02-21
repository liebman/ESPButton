/*
 * Config.h
 *
 *  Created on: Dec 3, 2017
 *      Author: chris.l
 */

#ifndef CONFIG_H_
#define CONFIG_H_

class Config {
public:
	Config();
	virtual     ~Config();
	void        begin();
	const char* getURL();
	void        setURL(const char* server);
    const char* getNTPServer();
    void        setNTPServer(const char* server);

	bool        save();
	bool        load();

private:
	char*    _url;
	char*    _ntp_server;
};

#endif /* CONFIG_H_ */
