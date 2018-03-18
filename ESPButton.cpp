#include "ESPButton.h"
#define DEBUG
#include "Logger.h"

Ticker timer;
RGBLED led(PIN_RED, PIN_GREEN, PIN_BLUE, CommonElectrode::anode);
Config config;

volatile ColorBlink color_blink;
volatile bool armed;
Ticker debounce;
volatile bool triggered;

bool force_config;

void setColor(const RGBColor *c)
{
    led(c->red, c->green, c->blue);
    led.show();
}

void blinkHandler()
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

void alternateColors(const RGBColor *on_color, float on_time,
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
    alternateColors(&blue, 0.125, &black, 20.0);
}

void armedColor()
{
    alternateColors(&yellow, 0.25, &black, 0.25);
}

void triggeredColor()
{
    alternateColors(&green, 0.125, &black, 0.125);
}

void failureColor()
{
    timer.detach();
    setColor(&red);
}

void successColor()
{
    timer.detach();
    setColor(&green);
}

void doIT()
{
    const char* url = config.getURL();
    const char* fingerprint = config.getFingerprint();

    HTTPClient http;
    http.setUserAgent("ESPButton/1.1");

    if (fingerprint && *fingerprint)
    {
        http.begin(url, fingerprint);
    }
    else
    {
        http.begin(url);
    }

    dbprintf("doIT: Starting Request: '%s'\n", config.getURL());
    int code = http.GET();
    dbprintf("doIT http code: %d\n", code);
    dbprintf("doIT: size: %d\n", http.getSize());

    http.end();

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
void debounceArmDisarm(int state)
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
void arm()
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
void trigger()
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

    WiFiManagerParameter url("URL", "URL", config.getURL(), 1024);
    wm.addParameter(&url);
    WiFiManagerParameter fingerprint("Fingerprint", "Fingerprint", config.getFingerprint(), 128);
    wm.addParameter(&fingerprint);

    WiFiManagerParameter otaurl("OTAURL", "OTA URL", "", 1024);
    wm.addParameter(&otaurl);
    WiFiManagerParameter otafp("OTAFingerprint", "OTA FP", "*", 128);
    wm.addParameter(&otafp);

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

    if (save_config)
    {
        config.setURL(url.getValue());
        config.setFingerprint(fingerprint.getValue());
        config.save();

        const char* ota_url = otaurl.getValue();
        const char* ota_fp  = otafp.getValue();
        dbprintf("OTA settings:  url: '%s' fp: '%s'\n", ota_url, ota_fp);

        if (ota_url != NULL && *ota_url != '\0')
        {
            dbprintf("OTA Update: from %s\n", ota_url);
            t_httpUpdate_return ret = HTTP_UPDATE_FAILED;
            if (strncmp(ota_url, "https:", 6) == 0)
            {
                ret = ESPhttpUpdate.update(ota_url, ESP_BUTTON_VERSION, otafp.getValue());
            }
            else
            {
                ret = ESPhttpUpdate.update(ota_url, ESP_BUTTON_VERSION);
            }

            switch(ret)
            {
                case HTTP_UPDATE_FAILED:
                    dbprintln("OTA update failed!");
                    break;
                case HTTP_UPDATE_NO_UPDATES:
                    dbprintln("OTA no updates!");
                    break;
                case HTTP_UPDATE_OK:
                    dbprintln("OTA update OK!"); // may not be reached as ESP is restarted!
                    break;
                default:
                    dbprintf("OTA update WTF? unexpected return code: %d\n", ret);
                    break;
            }
        }
    }

}

void setup()
{
    dbbegin(115200);
    dbprintln("\n\n\nStartup!");
    dbprintf("Version: %s\n", ESP_BUTTON_VERSION);

    pinMode(PIN_TRGR,     INPUT_PULLUP);
    pinMode(PIN_ARM,      INPUT_PULLUP);

    setColor(&blue);

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

    configTime(0, 0, DEFAULT_NTP_SERVER, NULL, NULL);

    dbprintf("setup: url:         '%s'\n", config.getURL());
    dbprintf("setup: fingerprint: '%s'\n", config.getFingerprint());

    armed     = false;
    triggered = false;

    readyColor();
    attachInterrupt(digitalPinToInterrupt(PIN_TRGR), &trigger, FALLING);
    attachInterrupt(digitalPinToInterrupt(PIN_ARM), &arm, CHANGE);
}

void loop()
{
    if (triggered)
    {
        dbprintf("TRIGGERED: millis: %lu\n", millis());
        doIT();
        triggered = false;
    }
    delay(10);
}
