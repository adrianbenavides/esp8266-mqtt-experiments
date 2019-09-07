#include <Arduino.h>

#define LED_PIN D1
#define BUTTON_PIN D2

#include "nodemcu.h"
const bool formatDevice = false;
const char *mcuSettings = "";
NodeMcu nodeMcu{mcuSettings};

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "mqtt.h"
const char *mqttServerAddress = "192.168.1.42";
uint mqttPort = 1883;
std::vector<std::string> mqttSubscriptions = {"/leds/on", "/leds/off"};
WiFiClient wifiClient;
PubSubClient pubsub(wifiClient);
Mqtt mqtt{mqttServerAddress, mqttPort, mqttSubscriptions};

void mqttOnMessageReceived(char *rtopic, byte *rpayload, unsigned int length);
void ledsMessageListener(std::string topic, std::string payload);
void buttonPressedListener();

void mqttOnMessageReceived(char *rtopic, byte *rpayload, unsigned int length)
{
    std::string topic = std::string(rtopic);
    std::string payload = mqtt.payloadToString(rpayload, length);
    Serial.printf("Message arrived. Topic: %s, payload: %s\n", topic.c_str(), payload.c_str());

    if (topic.find("leds") != std::string::npos)
    {
        ledsMessageListener(topic, payload);
    }
}

void ledsMessageListener(std::string topic, std::string payload)
{
    String deviceIdAsString(nodeMcu.deviceId);
    if (strcmp(payload.c_str(), deviceIdAsString.c_str()) == 0)
        return;

    if (topic.find("on") != std::string::npos)
    {
        digitalWrite(LED_PIN, HIGH);
    }
    else
    {
        digitalWrite(LED_PIN, LOW);
    }
}

String topic = "/leds/";
String lastLedAction = "on";
long lastTimePressed = millis();
void buttonPressedListener()
{
    long now = millis();
    if (now - lastTimePressed < 1000)
        return;

    if (digitalRead(BUTTON_PIN) == HIGH)
        return;

    if (!pubsub.connected())
        return;

    Serial.println(F("Button pressed"));

    delay(10);
    lastTimePressed = now;
    if (lastLedAction == "on")
    {
        lastLedAction = "off";
    }
    else
    {
        lastLedAction = "on";
    }

    String topic = "/leds/" + lastLedAction;
    String deviceIdAsString(nodeMcu.deviceId);
    Serial.printf("Publish message: %s, son topic: %s\n", deviceIdAsString.c_str(), topic.c_str());
    pubsub.publish(topic.c_str(), deviceIdAsString.c_str());
}

void setup()
{
    Serial.begin(115200);
    Serial.println();
    randomSeed(micros());

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    pinMode(BUTTON_PIN, OUTPUT);
    digitalWrite(BUTTON_PIN, HIGH);
    
    nodeMcu.setup(formatDevice);
    mqtt.setup(pubsub, mqttOnMessageReceived);
}

void loop()
{
    mqtt.loop(pubsub);
    buttonPressedListener();
}