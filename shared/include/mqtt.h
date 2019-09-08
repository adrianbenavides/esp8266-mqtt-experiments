#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

typedef struct {
    std::string topic;
    std::string message;
    uint qos;
    bool retain;
} MqttLastWill;

class Mqtt
{
public:
    Mqtt(const char *serverAddress, uint port, std::vector<std::string> &subscriptions);
    Mqtt(const char *serverAddress, uint port, std::vector<std::string> &subscriptions, MqttLastWill &lastWill);

    void setup(PubSubClient &client, std::function<void(char *, uint8_t *, unsigned int)>, uint deviceId);
    void loop(PubSubClient &client);
    std::string payloadToString(byte *payload, unsigned int length);

private:
    const char *serverAddress;
    uint port;
    std::vector<std::string> subscriptions;
    MqttLastWill lastWill;
    bool hasLastWill;
    std::string clientId;

    void init(const char *serverAddress, uint port, std::vector<std::string> &subscriptions);
};