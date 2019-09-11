#include "mqtt.h"

Mqtt::Mqtt(const char *serverAddress, uint port, std::vector<std::string> &subscriptions)
{
    this->init(serverAddress, port, subscriptions);
}

Mqtt::Mqtt(const char *serverAddress, uint port, std::vector<std::string> &subscriptions, MqttLastWill &lastWill)
{
    this->init(serverAddress, port, subscriptions);
    this->lastWill = lastWill;
    this->hasLastWill = true;
}

void Mqtt::init(const char *serverAddress, uint port, std::vector<std::string> &subscriptions)
{
    this->serverAddress = serverAddress;
    this->port = port;
    this->subscriptions = subscriptions;
    this->hasLastWill = false;
}

void Mqtt::setup(PubSubClient &pubsub, std::function<void(char *, uint8_t *, unsigned int)> callback, uint deviceId)
{
    pubsub.setServer(this->serverAddress, this->port);
    pubsub.setCallback(callback);

    String clientId = String(deviceId);
    this->clientId = std::string(clientId.c_str());

    if(this->hasLastWill) 
    {
        String topic = String(this->lastWill.topic.c_str());
        topic.replace("+", clientId);
        this->lastWill.topic = topic.c_str();
    }
}

void Mqtt::loop(PubSubClient &pubsub)
{
    if (!pubsub.connected())
    {
        while (!pubsub.connected())
        {
            String mqttId = "device-" + String(this->clientId.c_str()) + String("-") + String(random(0xffff), HEX);
            Serial.printf("Attempting MQTT connection for device %s", mqttId.c_str());
            
            bool connected = false;
            if(this->hasLastWill) {
                Serial.println(" with last will");
                connected = pubsub.connect(
                    mqttId.c_str(), 
                    this->lastWill.topic.c_str(), 
                    this->lastWill.qos, 
                    this->lastWill.retain, 
                    this->lastWill.message.c_str()
                );
            } else {
                Serial.println(" without last will");
                connected = pubsub.connect(mqttId.c_str());
            }

            if (connected)
            {
                Serial.println("connected");
                if(this->hasLastWill) {
                    Serial.printf("Last will message published to topic %s\n", this->lastWill.topic.c_str());
                    pubsub.publish(this->lastWill.topic.c_str(), "1", true);
                }

                for (std::string &topic : this->subscriptions)
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
    for (uint i = 0; i < length; i++)
    {
        msg += (char)payload[i];
    }

    return msg;
}