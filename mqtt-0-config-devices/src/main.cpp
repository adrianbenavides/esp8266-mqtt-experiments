#include "nodemcu.h"
const bool formatDevice = true;
const char *mcuSettings = "{\"device_id\":2,\"port\":80,\"ssid\":\"WIFI_SSID\",\"password\":\"WIFI_PASS\"}";
NodeMcu nodeMcu{mcuSettings};

void setup()
{
    Serial.begin(115200);
    Serial.println();

    randomSeed(micros());
    nodeMcu.setup(formatDevice);
}

void loop()
{
    delay(500);
}