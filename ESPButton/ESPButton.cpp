#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <FS.h>
#include <time.h>
#include <exception>
#include <WiFiManager.h>
#include "Config.h"
#include "HTTP.h"
#include "RGBLED.h"
#include "RGBSeq.h"
#include "PolledButton.h"
#include "CodeUpdate.h"
#include "AudioOutputI2SNoDAC.h"
#include <ESP8266SAM.h>
#include "Syslog.h"
#include <stdarg.h>

#define USE_WIFI 1
#define USE_SYSLOG 1
#define USE_UPDATE 1
#define USE_AUDIO 1


#define PIN_RED      12
#define PIN_GREEN    13
#define PIN_BLUE     16
#define PIN_ARM      0
#define PIN_TRIGGER  14
#define COLOR_BLACK  {0,   0,   0}
#define COLOR_RED    {255, 0,   0}
#define COLOR_GREEN  {0,   255, 0}
#define COLOR_BLUE   {0,   0,   255}
#define COLOR_YELLOW {255, 255, 0}
#define COLOR_ORANGE {255, 127, 0}

static const char     DEFAULT_URL[]        = "https://jigsaw.w3.org/HTTP/connection.html"; // "https://jigsaw.w3.org/HTTP/300/301.html";
static const char     DEFAULT_NTP_SERVER[] = "0.zoddotcom.pool.ntp.org";

enum class State
{
    BOOT,
    CONNECT,
    CONFIG,
    IDLE,
    ARMED,
    ACTIVE,
    SUCCESS,
    FAIL
};

Config               config;
PolledButton         arm(PIN_ARM);
PolledButton         trigger(PIN_TRIGGER);
RGBLED               rgb(PIN_RED, PIN_GREEN, PIN_BLUE);
RGBSeq               seq(rgb, 4);
State                state = State::BOOT;
#if USE_UPDATE
CodeUpdate           update(PIN_RED);
#endif
#if USE_SYSLOG
WiFiUDP             udp;
Syslog              slog(udp);
bool                sysloginit = false;
#endif

#ifndef DBG_MAIN
#define DBG_MAIN 1
#endif
#if DBG_MAIN
#if USE_SYSLOG
#if 1
#define DBG(fmt, ...) DBG_P( (PGM_P)PSTR(fmt), ## __VA_ARGS__ )
void DBG_P(const char* fmt, ...)
{
    va_list ap;

    if (sysloginit)
    {
        va_start(ap, fmt);
        slog.vlogf_P(LOG_INFO, fmt, ap);
        va_end(ap);
    }

    va_start(ap, fmt);
    char temp[64];
    char* buffer = temp;
    size_t len = vsnprintf_P(temp, sizeof(temp), fmt, ap);
    va_end(ap);
    if (len > sizeof(temp) - 1) {
        buffer = new char[len + 1];
        if (!buffer) {
            return;
        }
        va_start(ap, fmt);
        vsnprintf_P(buffer, len + 1, fmt, ap);
        va_end(ap);
    }
    len = Serial.write((const uint8_t*) buffer, len);
    if (buffer != temp) {
        delete[] buffer;
    }
}
#else
#define DBG(fmt, ...) {if (udp) slog.logf_P(LOG_INFO, (PGM_P)PSTR(fmt), ## __VA_ARGS__ ); else Serial.printf_P( (PGM_P)PSTR(fmt), ## __VA_ARGS__ );}
#endif
#else
#define DBG(fmt, ...) Serial.printf_P( (PGM_P)PSTR(fmt), ## __VA_ARGS__ )
#endif
#define DBG_FLUSH()   Serial.flush()
#else
#define DBG(...)
#define DBG_FLUSH()
#endif

#if USE_AUDIO
void say(const char* speach)
{
    AudioOutputI2SNoDAC out;
    ESP8266SAM sam;
    sam.SetVoice(ESP8266SAM::SAMVoice::VOICE_ELF);
    DBG("saying '%s'\n", speach);
    sam.Say(&out, speach);
}
#else
#define say(v)
#endif

void printMemInfo(const char* prefix = "")
{
    DBG("%smemory free: %u maxchunk:%u\n", prefix, ESP.getFreeHeap(), ESP.getMaxFreeBlockSize());
}

void setStateColor()
{
    switch(state)
    {
        case State::BOOT:
            seq.stop();
            rgb.set(COLOR_BLUE);
            break;

        case State::CONNECT:
            seq.run({
                RGBSeqItem(COLOR_BLUE,  0.125f),
                RGBSeqItem(COLOR_BLACK, 0.125f)
            });
            break;

        case State::CONFIG:
            seq.run({
                RGBSeqItem(COLOR_RED,   0.125f),
                RGBSeqItem(COLOR_GREEN, 0.125f)
            });
            break;

        case State::IDLE:
            seq.run({
                RGBSeqItem(COLOR_BLUE,  0.1f),
                RGBSeqItem(COLOR_BLACK, 300.0f)
            });
            break;

        case State::ARMED:
            seq.run({
                RGBSeqItem(COLOR_YELLOW, 0.25f),
                RGBSeqItem(COLOR_BLACK,  0.25f)
            });
            break;

        case State::ACTIVE:
            seq.run({
                RGBSeqItem(COLOR_GREEN, 0.125f),
                RGBSeqItem(COLOR_BLACK, 0.125f)
            });
            break;

        case State::SUCCESS:
            seq.stop();
            rgb.set(COLOR_GREEN);
            break;

        case State::FAIL:
            seq.stop();
            rgb.set(COLOR_RED);
            break;
    }

}

const char* getStateName(State state)
{
    const char* name = "UNKNOWN";
    switch(state)
    {
        case State::BOOT:
            name = "BOOT";
            break;
        case State::CONNECT:
            name = "CONNECT";
            break;
        case State::CONFIG:
            name = "CONFIG";
            break;
        case State::IDLE:
            name = "IDLE";
            break;
        case State::ARMED:
            name = "ARMED";
            break;
        case State::ACTIVE:
            name = "ACTIVE";
            break;
        case State::SUCCESS:
            name = "SUCCESS";
            break;
        case State::FAIL:
            name = "FAIL";
            break;
    }
    return name;
}

void setState(State new_state)
{
    if (state == new_state)
    {
        return;
    }

    DBG("setState: %s -> %s\n", getStateName(state), getStateName(new_state));
    state = new_state;
    setStateColor();
    printMemInfo();
}

// Set time via NTP, as required for x.509 validation
void setClock()
{
    const char* ntp_server = config.getNTPServer();
    IPAddress address;
    while(!WiFi.hostByName(ntp_server, address))
    {
        DBG("FAILED to lookup address for: '%s', retrying...\n", ntp_server);
        delay(1000);
    }

    DBG("DNS lookup gives: %d.%d.%d.%d\n", address[0], address[1], address[2], address[3]);

    DBG("setting NTP server to '%s'\n", ntp_server);
    configTime(0, 0, ntp_server, nullptr, nullptr);

    DBG("Waiting for NTP time sync...\n");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2)
    {
        delay(500);
        now = time(nullptr);
    }
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    DBG("Current UTC time: %s", asctime(&timeinfo));
}

void initWiFi(bool force)
{
    setState(State::CONNECT);

    if (!force)
    {
        WiFi.begin(/*SSID_NAME, SSID_PASS*/);
        // wait 30 up to seconds for connect
        uint32_t end = millis()+30000;
        DBG("wait for connect.\n");
        while ((WiFi.status() != WL_CONNECTED) && (millis() < end)) {
          delay(1000);
        }
    }

    printMemInfo();


    // if we are still not connected or force is true then start WiFiManager
    if (force || (WiFi.status() != WL_CONNECTED))
    {
        DBG("create WiFiManager\n");
        WiFiManager wm;
        printMemInfo();
        DBG("adding parameters\n");
        WiFiManagerParameter url("url", "URL", config.getURL(), 1024);
        wm.addParameter(&url);
        WiFiManagerParameter ntp("ntp", "NTP Server", config.getNTPServer(), 64);
        wm.addParameter(&ntp);
#if USE_UPDATE
        WiFiManagerParameter upd("update", "update url", "", 1024);
        wm.addParameter(&upd);
#endif
        printMemInfo();
        DBG("setting config save callback\n");
        wm.setAPCallback([](WiFiManager *)
        {
            DBG("config portal up!\n");
            printMemInfo();
            setState(State::CONFIG);
        });
        wm.setSaveConfigCallback([&]()
        {
            DBG("save config callback!");
            printMemInfo();
            DBG("saving config!");
            config.setURL(url.getValue());
            config.setNTPServer(ntp.getValue());
            config.save();
#if USE_UPDATE
            if (!update.setUpdate(upd.getValue()))
            {
                DBG("Failed to set update URL!!!!\n");
            }
#endif
            printMemInfo();
        });
        printMemInfo();
        DBG("starting config portal\n");
        wm.startConfigPortal("ESP_BUTTON", nullptr);
        printMemInfo();
        DBG("\n\n************************** RESETTING!!!!!!\n\n");
        DBG_FLUSH();
        ESP.restart();
    }
}

void maybeUpdate()
{
#if USE_UPDATE
    if (update.isUpdate())
    {
        DBG("starting update\n");
        seq.stop();
        rgb.set(COLOR_BLACK);
        if (!update.update())
        {
            DBG("update FAILED\n");
            seq.run({
                RGBSeqItem(COLOR_RED,   0.125f),
                RGBSeqItem(COLOR_BLACK, 0.125f)
            });
            while(true)
            {
                delay(1000);
            }
        }
        DBG("\n\n************************** RESETTING!!!!!!\n\n");
        DBG_FLUSH();
        ESP.restart();
    }
#endif
}

void setup()
{
    setStateColor();
    Serial.begin(76800);
    for(int i = 0; i < 5;++i) {Serial.println(i); delay(1000);}
    printMemInfo();

    DBG("Starting SPIFFS\n");
    SPIFFS.begin();
    printMemInfo();

    maybeUpdate();

#if USE_AUDIO
    DBG("Starting SAM\n");
    printMemInfo();
#endif

    DBG("Starting Config\n");
    config.begin();
    printMemInfo();

    DBG("loading config\n");
    if (!config.load())
    {
      DBG("failed to load config, setting defaults!\n");
      config.setURL(DEFAULT_URL);
      config.setNTPServer(DEFAULT_NTP_SERVER);
      if (!config.save())
      {
          DBG("Failed to save config!!!\n");
      }
    }
    printMemInfo();

#if USE_WIFI
    DBG("Starting WiFi\n");
    initWiFi(trigger.isPressedNow());
    printMemInfo();
#if USE_SYSLOG
    DBG("Starting Syslog\n");
    slog.server("192.168.1.17", 514);
    slog.defaultPriority(LOG_USER);
    sysloginit = true;
    printMemInfo();
#endif
#endif

    setState(State::IDLE);
    say(" the system ready");
}


void loop()
{
    uint32 armed     = arm.isPressed();
    uint32 triggered = trigger.isPressed();

    switch (state)
    {
        case State::BOOT:
        case State::CONFIG:
        case State::CONNECT:
        case State::IDLE:
            if (armed && !triggered)
            {
                setState(State::ARMED);
                say(" armed");
            }
            break;

        case State::ARMED:
            if (armed == 0)
            {
                setState(State::IDLE);
                say(" dis-armed");
            }
            else if (armed < triggered)
            {
                // if the trigger time is after the arm time then trigger!
                setState(State::ACTIVE);
                say(" activated");
                // triggered!
#if USE_WIFI
                HTTP http;
                if (http.GET(config.getURL()))
                {
                    say(" success");
                    setState(State::SUCCESS);
                }
                else
                {
                    say(" the request failed");
                    setState(State::FAIL);
                }
#endif
            }
            break;

        case State::ACTIVE:
        case State::SUCCESS:
        case State::FAIL:
            if (armed == 0 && triggered == 0)
            {
                setState(State::IDLE);
                say(" dis-armed");
            }
            break;
    }
}
