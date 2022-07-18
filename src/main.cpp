
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <TaskScheduler.h>
#include "conf.h"
#include "ap.h"
#include "sta.h"

Scheduler runner;
AsyncWebServer server(80);
STA sta;
AP ap;
int enbleNatFlag = 0;
void lightToggle()
{
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}
void enableNat()
{
	if (WiFi.status() == WL_CONNECTED && enbleNatFlag == 0)
	{
		ap.ipNaptEnable();
		enbleNatFlag = 1;
	}
}
Task blinkTask(500, TASK_FOREVER, &lightToggle);
Task restartTask(0, TASK_ONCE, &ESP.restart);
Task enableNatTask(1000, 10, &enableNat);
void infoHandler(AsyncWebServerRequest *request)
{
	AsyncResponseStream *response = request->beginResponseStream("application/json");
	DynamicJsonDocument json(1024);
	json["status"] = WiFi.status() == WL_CONNECTED ? "已联网" : "未联网";
	json["ssid"] = WiFi.softAPSSID();
	json["mac"] = WiFi.softAPmacAddress();
	serializeJson(json, *response);
	request->send(response);
}
void configHandler(AsyncWebServerRequest *request, JsonVariant &json)
{
	DynamicJsonDocument data(1024);
	if (json.is<JsonArray>())
	{
		data = json.as<JsonArray>();
	}
	else if (json.is<JsonObject>())
	{
		data = json.as<JsonObject>();
	}
	String response;
	serializeJson(data, response);
	writeConfig(response);
	Serial.println(response);
	Serial.println(readConfig());
	request->send(200, "application/json", response);
	restartTask.enableDelayed(2000);
}
void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	Serial.begin(115200);
	enableConfig();
	runner.init();
	runner.addTask(blinkTask);
	runner.addTask(restartTask);
	runner.addTask(enableNatTask);
	if (haveConfig())
	{
		server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
		String conf = readConfig();
		DynamicJsonDocument doc(1024);
		deserializeJson(doc, conf);
		String ssid = doc["ssid"];
		String password = doc["password"];
		String apssid = doc["apssid"];
		const char *apmac = doc["apmac"];
		sta.connect(ssid, password);
		enableNatTask.enable();
		ap.setMac(apmac);
		ap.init(apssid);
		Serial.println(WiFi.softAPSSID());
		Serial.println(WiFi.softAPmacAddress());
		digitalWrite(LED_BUILTIN, LOW);
	}
	else
	{
		server.serveStatic("/", LittleFS, "/").setDefaultFile("config.html");
		ap.init("ding.crack");
		blinkTask.enable();
	}
	server.on("/api/info", HTTP_GET, infoHandler);
	AsyncCallbackJsonWebHandler *confingRoute = new AsyncCallbackJsonWebHandler("/api/config", configHandler);
	server.addHandler(confingRoute);
	server.begin();
}

void loop()
{
	runner.execute();
}