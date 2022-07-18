#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <lwip/napt.h>
#include <lwip/dns.h>
#include <LwipDhcpServer.h>

// ap类
class AP
{
public:
    void init(String ssid);
    void ipNaptEnable();
    bool setMac(const char *);
};

void AP::init(String ssid)
{
    dhcpSoftAP.dhcps_set_dns(0, IPAddress(223, 5, 5, 5));
    dhcpSoftAP.dhcps_set_dns(1, IPAddress(223, 6, 6, 6));
    WiFi.softAPConfig(
        IPAddress(192, 168, 4, 1),
        IPAddress(192, 168, 4, 1),
        IPAddress(255, 255, 255, 0));
    WiFi.softAP(ssid);
};

void AP::ipNaptEnable()
{
    err_t ret = ip_napt_init(1000, 10);
    if (ret == ERR_OK)
    {
        ret = ip_napt_enable_no(SOFTAP_IF, 1);
        if (ret == ERR_OK)
        {
            Serial.println("WiFi router is running!");
        }
    }
    if (ret != ERR_OK)
    {
        Serial.println("NAPT initialization failed");
    }
};
bool AP::setMac(const char *mac)
    
{
    uint8_t values[6] = {0};
    sscanf(mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]);
    bool success = wifi_set_macaddr(SOFTAP_IF, &values[0]);
    return success;
    
};
