#include <PubSubClient.h>
#include "mqtt.h"

Mqtt::Mqtt(const char *mqttServerAddress, uint mqttPort, std::vector<std::string> mqttSubscriptions)
{
    this->mqttServerAddress = mqttServerAddress;
    this->mqttPort = mqttPort;
    this->mqttSubscriptions = mqttSubscriptions;
}

void Mqtt::setup(PubSubClient &pubsub, std::function<void(char *, uint8_t *, unsigned int)> callback)
{
    pubsub.setServer(this->mqttServerAddress, this->mqttPort);
    pubsub.setCallback(callback);
}

void Mqtt::loop(PubSubClient &pubsub)
{
    if (!pubsub.connected())
    {
        while (!pubsub.connected())
        {
            Serial.print("Attempting MQTT connection...");
            String clientId = "ESP8266Client-";
            clientId += String(random(0xffff), HEX);
            if (pubsub.connect(clientId.c_str()))
            {
                Serial.println("connected");
                for (std::string &topic : this->mqttSubscriptions)
                {
                    pubsub.subscribe(topic.c_str());
                    Serial.printf("Subscribed to topic: %s\n", topic.c_str());
                }
            }
            else
            {
                Serial.print("failed, rc=");
                Serial.print(pubsub.state());
                Serial.println(" try again in 5 seconds");
                delay(5000);
            }
        }
    }
    pubsub.loop();
}

std::string Mqtt::payloadToString(byte *payload, unsigned int length)
{
    std::string msg = "";
    for (int i = 0; i < length; i++)
    {
        msg += (char)payload[i];
    }

    return msg;
}