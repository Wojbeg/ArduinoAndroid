#include "message.h"
#include "transferData.h"

// Message::Message(float temperature, float humidity, boolean isLedOn, int colors[3])
//     : temperature(temperature), humidity(humidity), isLedOn(isLedOn)
// {
//     for (int i = 0; i < 3; ++i) {
//         this->colors[i] = colors[i];
//     }
// }

Message::Message(float temperature, float humidity, bool isLedOn, String ip, const std::initializer_list<int> & colors)
    : temperature(temperature), humidity(humidity), isLedOn(isLedOn), ip(std::move(ip))
{
    int i = 0;
    for (auto color : colors) {
        this->colors[i++] = color;
    }
}

void Message::toJson(JsonDocument& json, String& jsonString) {
    json["temperature"] = getTemperature();
    json["humidity"] = getHumidity();
    json["isLedOn"] = getIsLedOn();
    json["ip"] = getIp();

    for(int i = 0; i < COLORS_NUMBER; i++)
        json["colors"][i] = colors[i];

    serializeJson(json, jsonString);
}

void Message::toJsonString(String name, JsonDocument& json, String& jsonString) {
    json[name]["temperature"] = getTemperature();
    json[name]["humidity"] = getHumidity();
    json[name]["isLedOn"] = getIsLedOn();
    json[name]["ip"] = getIp();

    for(int i = 0; i < COLORS_NUMBER; i++)
        json[name]["colors"][i] = colors[i];
}

void Message::addObjectToJson(JsonDocument& json, String& jsonString) {
    JsonObject obj = json.createNestedObject("message");

    obj["temperature"] = getTemperature();
    obj["humidity"] = getHumidity();
    obj["isLedOn"] = getIsLedOn();
    obj["ip"] = getIp();

    for(int i = 0; i < COLORS_NUMBER; i++)
        obj["colors"][i] = colors[i];
}


// void Message::serialize(char* jsonString) {
//   StaticJsonDocument<200> doc;
//   JsonObject root = doc.to<JsonObject>();

//   root["temperature"] = getTemperature();
//   root["humidity"] = getHumidity();
//   root["isLedOn"] = getIsLedOn();
//   const int* colors = getColors();
//   root["colors"][0] = colors[0];
//   root["colors"][1] = colors[1];
//   root["colors"][2] = colors[2];

//   serializeJson(doc, jsonString);
// }

// void Message::serialize(String& jsonString) {
//   StaticJsonDocument<200> doc;
//   JsonObject root = doc.to<JsonObject>();

//   root["temperature"] = getTemperature();
//   root["humidity"] = getHumidity();
//   root["isLedOn"] = getIsLedOn();
//   const int* colors = getColors();
//   root["colors"][0] = colors[0];
//   root["colors"][1] = colors[1];
//   root["colors"][2] = colors[2];

//   serializeJson(doc, jsonString);
// }