#ifndef MESSAGE_H
#define MESSAGE_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <transferData.h>
#include <array>

class Message : public TransferData {
    private:
        volatile float temperature;
        volatile float humidity;
        std::array<int, 3> colors;
        bool isLedOn;
        const String ip;

        static constexpr size_t COLORS_NUMBER = 3;
    public:
        Message(float temperature, float humidity, bool isLedOn, String ip, const std::initializer_list<int> & colors);

        [[nodiscard]] FORCE_INLINE float getTemperature() const {return temperature;}
        [[nodiscard]] FORCE_INLINE float getHumidity() const {return humidity;}
        [[nodiscard]] FORCE_INLINE bool getIsLedOn() const {return isLedOn;}
        [[nodiscard]] FORCE_INLINE const std::array<int,COLORS_NUMBER> & getColors() const {return colors;}
        [[nodiscard]] FORCE_INLINE const String & getIp() const {return ip;}

        void toJson(JsonDocument& json, String& jsonString);
        void toJsonString(String name, JsonDocument& json, String& jsonString) override;
        void addObjectToJson(JsonDocument& json, String& jsonString) override;
};

#endif