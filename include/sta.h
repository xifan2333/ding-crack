#include <Arduino.h>
#include <ESP8266WiFi.h>
// sta类

class STA
{
public:
    void connect(String ssid,String password);
};
void STA::connect(String ssid,String password)
{
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(ssid, password);
}