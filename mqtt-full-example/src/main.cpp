#define LED_PIN D1
#define BUTTON_PIN D2

#include "nodemcu.h"
NodeMcu nodeMcu;

#include "mqtt.h"
const char *mqttServerAddress = "192.168.1.43";
uint mqttPort = 1883;
std::vector<std::string> mqttSubscriptions = {"/client/+/led", "/client/+/status"};
WiFiClient wifiClient;
PubSubClient pubsub(wifiClient);
MqttLastWill will{"/client/+/status", "0", 0, true};
Mqtt mqtt{mqttServerAddress, mqttPort, mqttSubscriptions, will};

void mqttOnMessageReceived(char *rtopic, byte *rpayload, unsigned int length);
void ledsMessageListener(std::string topic, std::string payload);
void buttonPressedListener();
void lastWillMessageListener(std::string topic, std::string payload);

void setup()
{
    Serial.begin(115200);
    Serial.println();
    randomSeed(micros());

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    pinMode(BUTTON_PIN, OUTPUT);
    digitalWrite(BUTTON_PIN, HIGH);

    nodeMcu.setup();
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
    lastWillMessageListener(topic, payload);
}

void ledsMessageListener(std::string topic, std::string payload)
{
    if (topic.find("/client/") == std::string::npos
        || topic.find("/led") == std::string::npos)
        return;

    Serial.println("ledsMessageListener");
    String deviceIdAsString(nodeMcu.deviceId);
    if (topic.find(deviceIdAsString.c_str()) != std::string::npos)
        return;

    if (strcmp(payload.c_str(), "1") == 0)
        digitalWrite(LED_PIN, HIGH);
    else
        digitalWrite(LED_PIN, LOW);
}

void lastWillMessageListener(std::string topic, std::string payload)
{
    if (topic.find("/client/") == std::string::npos
        || topic.find("/status") == std::string::npos)
        return;

    Serial.println("lastWillMessageListener");
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
    Serial.printf("Publish message: %s, on topic: %s\n", String(lastLedStatus).c_str(), topic.c_str());
    pubsub.publish(topic.c_str(), String(lastLedStatus).c_str());
}
