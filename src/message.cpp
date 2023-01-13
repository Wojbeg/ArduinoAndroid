#include <message.h>
#include <transferData.h>

// Message::Message(float temperature, float humidity, boolean isLedOn, int colors[3])
//     : temperature(temperature), humidity(humidity), isLedOn(isLedOn)
// {
//     for (int i = 0; i < 3; ++i) {
//         this->colors[i] = colors[i];
//     }
// }

Message::Message(float temperature, float humidity, bool isLedOn, String ip, std::initializer_list<int> colors)
    : temperature(temperature), humidity(humidity), isLedOn(isLedOn), ip(ip)
{
    int i = 0;
    for (int color : colors) {
        this->colors[i++] = color;
    }
}

float Message::getTemperature() const {
    return temperature;
}

float Message::getHumidity() const {
    return humidity;
}

bool Message::getIsLedOn() const {
    return isLedOn;
}

const int* Message::getColors() const {
    return colors;
}

String Message::getIp() {
    return ip;
}

void Message::toJson(JsonDocument& json, String& jsonString) {
    json["temperature"] = getTemperature();
    json["humidity"] = getHumidity();
    json["isLedOn"] = getIsLedOn();
    json["ip"] = getIp();

    const int* colors = getColors();
    json["colors"][0] = colors[0];
    json["colors"][1] = colors[1];
    json["colors"][2] = colors[2];

    serializeJson(json, jsonString);
}

void Message::toJsonString(String name, JsonDocument& json, String& jsonString) {
    json[name]["temperature"] = getTemperature();
    json[name]["humidity"] = getHumidity();
    json[name]["isLedOn"] = getIsLedOn();
    json[name]["ip"] = getIp();

    const int* colors = getColors();
    json[name]["colors"][0] = colors[0];
    json[name]["colors"][1] = colors[1];
    json[name]["colors"][2] = colors[2];
}

void Message::addObjectToJson(JsonDocument& json, String& jsonString) {
    JsonObject obj = json.createNestedObject("message");

    obj["temperature"] = getTemperature();
    obj["humidity"] = getHumidity();
    obj["isLedOn"] = getIsLedOn();
    obj["ip"] = getIp();

    const int* colors = getColors();
    obj["colors"][0] = colors[0];
    obj["colors"][1] = colors[1];
    obj["colors"][2] = colors[2];
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