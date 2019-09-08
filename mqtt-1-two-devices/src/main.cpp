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
std::vector<std::string> mqttSubscriptions = {"/client/+/led"};
WiFiClient wifiClient;
PubSubClient pubsub(wifiClient);
Mqtt mqtt{mqttServerAddress, mqttPort, mqttSubscriptions};

void mqttOnMessageReceived(char *rtopic, byte *rpayload, unsigned int length);
void ledsMessageListener(std::string topic, std::string payload);
void buttonPressedListener();

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
    mqtt.setup(pubsub, mqttOnMessageReceived, nodeMcu.deviceId);
}

void loop()
{
    mqtt.loop(pubsub);
    buttonPressedListener();
}

void mqttOnMessageReceived(char *rtopic, byte *rpayload, unsigned int length)
{
    std::string topic = std::string(rtopic);
    std::string payload = mqtt.payloadToString(rpayload, length);
    Serial.printf("Message arrived. Topic: %s, payload: %s\n", topic.c_str(), payload.c_str());
    
    ledsMessageListener(topic, payload);
}

void ledsMessageListener(std::string topic, std::string payload)
{
    if (topic.find("/client/") == std::string::npos
        && topic.find("/led") == std::string::npos)
        return;

    String deviceIdAsString(nodeMcu.deviceId);
    if (topic.find(deviceIdAsString.c_str()) != std::string::npos)
        return;

    if (strcmp(payload.c_str(), "1") == 0)
        digitalWrite(LED_PIN, HIGH);
    else
        digitalWrite(LED_PIN, LOW);
}

bool lastLedStatus = 0;
long lastTimePressed = millis();
void buttonPressedListener()
{
    if (!pubsub.connected())
        return;

    long now = millis();
    if (now - lastTimePressed < 500)
        return;

    if (digitalRead(BUTTON_PIN) == HIGH)
        return;

    Serial.println(F("Button pressed"));

    delay(10);
    lastTimePressed = now;
    lastLedStatus = !lastLedStatus;

    String deviceIdAsString(nodeMcu.deviceId);
    String topic = "/client/" + deviceIdAsString + "/led";
    Serial.printf("Publish message: %s, son topic: %s\n", String(lastLedStatus).c_str(), topic.c_str());
    pubsub.publish(topic.c_str(), String(lastLedStatus).c_str());
}
