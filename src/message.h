#ifndef MESSAGE_H
#define MESSAGE_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <transferData.h>

class Message : public TransferData {
    private:
        float temperature;
        float humidity;
        int colors[3];
        boolean isLedOn;
        String ip;
    
    public:
        Message(float temperature, float humidity, boolean isLedOn, String ip, std::initializer_list<int> colors);

        float getTemperature() const;
        float getHumidity() const;
        bool getIsLedOn() const;
        const int* getColors() const;
        String getIp();

        void toJson(JsonDocument& json, String& jsonString);
        void toJsonString(String name, JsonDocument& json, String& jsonString) override;
        void addObjectToJson(JsonDocument& json, String& jsonString) override;
};

#endif