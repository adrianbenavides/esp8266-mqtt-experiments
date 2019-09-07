class NodeMcu
{
public:
    NodeMcu(const char *settings);

    uint deviceId;

    void setup(bool format);

private:
    const char *settings;
    uint port;
    const char *ssid;
    const char *password;

    void setupFilesystem(bool format);
    void deserializeSettings();
    void setupWifi();
};