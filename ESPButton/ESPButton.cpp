#include "ESPButton.h"
#include "Logger.h"
#include "DLogPrintWriter.h"

#include <FS.h>
#include <time.h>

BearSSL::CertStore certStore;

DLog& dlog = DLog::getLog();

Ticker timer;
RGBLED led(PIN_RED, PIN_GREEN, PIN_BLUE);
Config config;

volatile ColorBlink color_blink;
volatile bool armed;
Ticker debounce;
volatile bool triggered;

bool force_config;

class SPIFFSCertStoreFile : public BearSSL::CertStoreFile {
  public:
    SPIFFSCertStoreFile(const char *name) {
      _name = name;
    };
    virtual ~SPIFFSCertStoreFile() override {};

    // The main API
    virtual bool open(bool write = false) override {
      _file = SPIFFS.open(_name, write ? "w" : "r");
      return _file;
    }
    virtual bool seek(size_t absolute_pos) override {
      return _file.seek(absolute_pos, SeekSet);
    }
    virtual ssize_t read(void *dest, size_t bytes) override {
      return _file.readBytes((char*)dest, bytes);
    }
    virtual ssize_t write(void *dest, size_t bytes) override {
      return _file.write((uint8_t*)dest, bytes);
    }
    virtual void close() override {
      _file.close();
    }

  private:
    File _file;
    const char *_name;
};

SPIFFSCertStoreFile certs_idx("/certs.idx");
SPIFFSCertStoreFile certs_ar("/certs.ar");


// Set time via NTP, as required for x.509 validation
void setClock()
{
    const char* ntp_server = config.getNTPServer();
    IPAddress address;
    while(!WiFi.hostByName(ntp_server, address))
    {
        dlog.error("setClock", "FAILED to lookup address for: '%s', retrying...", ntp_server);
        delay(1000);
    }

    dlog.info("setClock", "DNS lookup gives: %d.%d.%d.%d", address[0], address[1], address[2], address[3]);

    dlog.info("setClock", "setting NTP server to '%s'", ntp_server);
    configTime(0, 0, ntp_server, nullptr, nullptr);

    dlog.info("setCock", "Waiting for NTP time sync...");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2)
    {
        dlog.trace("setClock", "now: %ul", now);
        delay(500);
        now = time(nullptr);
    }
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    dlog.info("setCock", "Current UTC time: %s", asctime(&timeinfo));
}


void ICACHE_RAM_ATTR setColor(const RGBColor *c)
{
    led.set(c);
}

void ICACHE_RAM_ATTR blinkHandler()
{
    if (color_blink.state)
    {
        setColor(color_blink.on_color);
        timer.once(color_blink.on_time, &blinkHandler);
    }
    else
    {
        setColor(color_blink.off_color);
        timer.once(color_blink.off_time, &blinkHandler);
    }
    color_blink.state = !color_blink.state;
}

void ICACHE_RAM_ATTR alternateColors(const RGBColor *on_color, float on_time,
        const RGBColor *off_color, float off_time)
{
    timer.detach();
    color_blink.on_color = on_color;
    color_blink.on_time = on_time;
    color_blink.off_color = off_color;
    color_blink.off_time = off_time;
    color_blink.state = false;
    blinkHandler();
}

void startupColor()
{
    timer.detach();
    setColor(&blue);
}

void configColor()
{
    alternateColors(&red, 0.125, &green, 0.125);
}

void readyColor()
{
    alternateColors(&blue, 0.1, &black, 300.0);
}

void updateColor()
{
    alternateColors(&blue, 0.5, &black, 0.5);
}

void ICACHE_RAM_ATTR armedColor()
{
    alternateColors(&yellow, 0.25, &black, 0.25);
}

void ICACHE_RAM_ATTR triggeredColor()
{
    alternateColors(&green, 0.125, &black, 0.125);
}

void ICACHE_RAM_ATTR failureColor()
{
    timer.detach();
    setColor(&red);
}

void ICACHE_RAM_ATTR successColor()
{
    timer.detach();
    setColor(&green);
}

bool ICACHE_RAM_ATTR startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

void doIT()
{
    const char* url = config.getURL();
    const char *keys[] = {"Location"};

    WiFiClient *client = nullptr;
    if (startsWith("https:", url))
    {
        BearSSL::WiFiClientSecure *bear  = new BearSSL::WiFiClientSecure();
        // Integrate the cert store with this connection
        bear->setCertStore(&certStore);
        client = bear;
    }
    else
    {
        client = new WiFiClient;
    }
    HTTPClient *http = new HTTPClient;
    http->setUserAgent("ESPButton/1.1");
    http->collectHeaders(keys, 1);
    http->begin(*client, url);

    dlog.info("doIt", "Starting Request: '%s'", config.getURL());
    int code = http->GET();
    dlog.info("doIt", "http code: %d", code);
    while (code == 302 || code == 301)
    {
        String location = http->header("Location");
        dlog.info("doIt", "Location: %s", location.c_str());
        http->end();
        if (http->setURL(location))
        {
            code = http->GET();
        }
        else
        {
            dlog.error("doIt", "Bad redirect location: %s", location.c_str());
            code = 500;
        }
        dlog.info("doIt", "redirect http code: %d", code);
    }
    dlog.info("doIt", "size: %d", http->getSize());

    http->end();

    delete http;
    delete client;

    if (code != 200)
    {
        failureColor();
        return;
    }

    successColor();
}

//
// If after the debounce timer expires the state is still the same then
// we can arm or disarm
//
void ICACHE_RAM_ATTR debounceArmDisarm(int state)
{
    if (digitalRead(PIN_ARM) == state)
    {
        if (state == 0)
        {
            armed = true;
            armedColor();
        }
        else
        {
            armed = false;
            readyColor();
        }
    }
}

//
// Called on change of ARM pin, use debounce timer to debounce state change.
//
void ICACHE_RAM_ATTR arm()
{
    //
    // Use a timer to debounce, could be arm or disarm
    //
    int state = digitalRead(PIN_ARM);
    debounce.once_ms(DEBOUNCE_DELAY, debounceArmDisarm, state);
}

//
// Called on falling edge of trigger pin, no debounce here as we mark as disarmed here
// and  the action (web request) will take longer than the bouncing.
//
void ICACHE_RAM_ATTR trigger()
{
    if (armed)
    {
        triggered = true;
        armed = false;
        triggeredColor();
    }
}

void initWifi()
{
    WiFiManager wm;
    wm.setDebugOutput(false);

    WiFiManagerParameter url("URL", "URL", config.getURL(), 1024);
    wm.addParameter(&url);
    WiFiManagerParameter ntp("ntp", "NTP Server", config.getNTPServer(), 64);
    wm.addParameter(&ntp);

    WiFiManagerParameter otaurl("OTAURL", "OTA URL", "", 1024);
    wm.addParameter(&otaurl);

    wm.setAPCallback([](WiFiManager *)
    {
        configColor();
    });

    static bool save_config  = false;
    wm.setSaveConfigCallback([]()
    {
        save_config = true;
    });

    String ssid = "ESPButton" + String(ESP.getChipId());

    if (force_config)
    {
        wm.startConfigPortal(ssid.c_str(), NULL);
    }
    else
    {
        wm.autoConnect(ssid.c_str(), NULL);
    }

    WiFi.mode(WIFI_STA);

    while (WiFi.status() != WL_CONNECTED) {
        dlog.warning("initWifi", "still not connected, waiting...");
        delay(1000);
    }

    if (save_config)
    {
        config.setURL(url.getValue());
        config.setNTPServer(ntp.getValue());
        config.save();

        const char* ota_url = otaurl.getValue();
        dlog.info("initWifi", "OTA settings:  url: '%s'", ota_url);

        if (ota_url != NULL && *ota_url != '\0')
        {
            dlog.info("initWifi", "OTA Update: from %s", ota_url);
            updateColor();
            ESPhttpUpdate.followRedirects(true);
            t_httpUpdate_return ret = HTTP_UPDATE_FAILED;

            WiFiClient *client = nullptr;
            if (strncmp(ota_url, "https:", 6) == 0)
            {
                BearSSL::WiFiClientSecure *bear  = new BearSSL::WiFiClientSecure();
                // Integrate the cert store with this connection
                bear->setCertStore(&certStore);
                client = bear;
            }
            else
            {
                client = new WiFiClient;
            }

            ret = ESPhttpUpdate.update(*client, ota_url, ESP_BUTTON_VERSION);

            switch(ret)
            {
                case HTTP_UPDATE_FAILED:
                    dlog.info("initWifi", "OTA update failed!");
                    break;
                case HTTP_UPDATE_NO_UPDATES:
                    dlog.info("initWifi", "OTA no updates!");
                    break;
                case HTTP_UPDATE_OK:
                    dlog.info("initWifi", "OTA update OK!"); // may not be reached as ESP is restarted!
                    break;
                default:
                    dlog.info("initWifi", "OTA update WTF? unexpected return code: %d", ret);
                    break;
            }

            delete client;
        }
    }
}

void setup()
{
    Serial.begin(76800);
    dlog.begin(new DLogPrintWriter(Serial));
    //dlog.setLevel(DLOG_LEVEL_DEBUG);
    dlog.info("setup", "Startup!");
    dlog.info("setup", "Version: %s", ESP_BUTTON_VERSION);

    pinMode(PIN_TRGR,     INPUT_PULLUP);
    pinMode(PIN_ARM,      INPUT_PULLUP);

    startupColor();

    if (SPIFFS.begin())
    {
        dlog.error("setup", "SPIFFS begin failed!!!");
    }

    config.begin();

    force_config = false;

    if (digitalRead(PIN_TRGR) == 0)
    {
        force_config = true;
    }

    if (!config.load())
    {
        force_config = true;
    }

    initWifi();
    startupColor();

    setClock();

    dlog.info("setup", "url:         '%s'", config.getURL());

    armed     = false;
    triggered = false;

    int numCerts = certStore.initCertStore(&certs_idx, &certs_ar);
    dlog.info("setup", "Number of CA certs read: %d", numCerts);
    if (numCerts == 0)
    {
      dlog.error("setup", "No certs found. Did you run certs-from-mozill.py and upload the SPIFFS directory before running?");
    }

    readyColor();
    attachInterrupt(digitalPinToInterrupt(PIN_TRGR), &trigger, FALLING);
    attachInterrupt(digitalPinToInterrupt(PIN_ARM), &arm, CHANGE);
}

void loop()
{
    if (triggered)
    {
        dlog.info("loop", "TRIGGERED: millis: %lu", millis());
        doIT();
        triggered = false;
    }
    delay(10);
}
