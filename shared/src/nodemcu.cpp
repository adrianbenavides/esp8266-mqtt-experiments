#include "nodemcu.h"

NodeMcu::NodeMcu()
{
    this->settings = "";
}

NodeMcu::NodeMcu(std::string &settings)
{
    this->settings = settings;
}

void NodeMcu::setup()
{
    this->setupFilesystem();
    this->deserializeSettings();
    this->setupWifi();
}

void NodeMcu::setupFilesystem()
{
    if (SPIFFS.begin())
    {
        Serial.println(F("Filesystem mounted."));
    }
    else
    {
        Serial.println(F("Mounting filesystem failed."));
        ESP.restart();
    }

    if (!this->settings.empty())
    {
        Serial.print(F("Formatting filesystem... "));
        if (SPIFFS.format())
        {
            Serial.println(F("done"));
        }
        else
        {
            Serial.println(F("error"));
            ESP.reset();
        }
    }
}

void NodeMcu::deserializeSettings()
{
    if (!SPIFFS.exists("/config.json"))
    {
        assert(!this->settings.empty());

        Serial.print(F("Creating config.json file... "));
        File configFile = SPIFFS.open("/config.json", "w");
        if (!configFile)
        {
            Serial.println(F("Error opening file"));
            ESP.reset();
        }

        configFile.write(this->settings.c_str());
        configFile.close();
        Serial.println(F("done"));
    }
    else
    {
        Serial.println(F("Reading existing config.json file"));
    }

    File configFile = SPIFFS.open("/config.json", "r");
    if (!configFile)
    {
        Serial.println(F("Error opening file"));
        ESP.reset();
    }

    Serial.print(F("Deserializing config.json... "));
    String configFileContent = configFile.readString();
    configFile.close();

    // Object size: https://arduinojson.org/v6/assistant/
    const size_t capacity = JSON_OBJECT_SIZE(4) + 130;
    DynamicJsonDocument doc(capacity);
    DeserializationError error = deserializeJson(doc, configFileContent.c_str());
    if (error)
    {
        Serial.println(F("config.json file couldn't be deserialized"));
        Serial.println(error.c_str());
        ESP.reset();
    }

    assert(doc.containsKey("device_id"));
    assert(doc.containsKey("port"));
    assert(doc.containsKey("ssid"));
    assert(doc.containsKey("password"));

    this->deviceId = doc["device_id"].as<uint>();
    this->port = doc["port"].as<uint>();
    this->ssid = doc["ssid"].as<char *>();
    this->password = doc["password"].as<char *>();
    Serial.println(F("done"));
    Serial.printf("Device id: %i\n", this->deviceId);
}

void NodeMcu::setupWifi()
{
    Serial.printf("Connecting to %s...", this->ssid);
    WiFi.begin(this->ssid, this->password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println(F(" done"));
}
