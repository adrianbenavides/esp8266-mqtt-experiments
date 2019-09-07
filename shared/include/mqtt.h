#include <PubSubClient.h>

class Mqtt
{
public:
    Mqtt(const char *mqttServerAddress, uint mqttPort, std::vector<std::string> _mqttSubscriptions);

    void setup(PubSubClient &client, std::function<void(char *, uint8_t *, unsigned int)>);
    void loop(PubSubClient &client);
    std::string payloadToString(byte *payload, unsigned int length);

private:
    const char *mqttServerAddress;
    uint mqttPort;
    std::vector<std::string> mqttSubscriptions;
};