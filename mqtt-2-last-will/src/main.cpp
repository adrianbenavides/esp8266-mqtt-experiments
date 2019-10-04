#define LED_PIN D1
#define MQTT_KEEPALIVE 15
#define MQTT_SOCKET_TIMEOUT 15

#include "nodemcu.h"
NodeMcu nodeMcu;

#include "mqtt.h"
const char *mqttServerAddress = "192.168.1.42";
uint mqttPort = 1883;
std::vector<std::string> mqttSubscriptions = {"/client/+/status"};
WiFiClient wifiClient;
PubSubClient pubsub(wifiClient);
MqttLastWill will{"/client/+/status", "0", 0, true};
Mqtt mqtt{mqttServerAddress, mqttPort, mqttSubscriptions, will};

void mqttOnMessageReceived(char *rtopic, byte *rpayload, unsigned int length);
void lastWillMessageListener(std::string topic, std::string payload);

void setup()
{
    Serial.begin(115200);
    Serial.println();
    randomSeed(micros());

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    nodeMcu.setup();
    mqtt.setup(pubsub, mqttOnMessageReceived, nodeMcu.deviceId);
}

void loop()
{
    mqtt.loop(pubsub);
}

void mqttOnMessageReceived(char *rtopic, byte *rpayload, unsigned int length)
{
    std::string topic = std::string(rtopic);
    std::string payload = mqtt.payloadToString(rpayload, length);
    Serial.printf("Message arrived. Topic: %s, payload: %s\n", topic.c_str(), payload.c_str());

    lastWillMessageListener(topic, payload);
}

void lastWillMessageListener(std::string topic, std::string payload)
{
    if (topic.find("/client/") == std::string::npos
        || topic.find("/status") == std::string::npos)
        return;

    String deviceIdAsString(nodeMcu.deviceId);
    if (topic.find(deviceIdAsString.c_str()) != std::string::npos)
        return;

    if (strcmp(payload.c_str(), "1") == 0)
        return;

    for (uint cycles = 0; cycles < 5; cycles++)
    {
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
        delay(200);
    }
}
