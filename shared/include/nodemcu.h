#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <assert.h>

class NodeMcu
{
public:
    NodeMcu();
    NodeMcu(std::string &settings);

    uint deviceId;

    void setup();

private:
    std::string settings;
    uint port;
    const char *ssid;
    const char *password;

    void setupFilesystem();
    void deserializeSettings();
    void setupWifi();
};