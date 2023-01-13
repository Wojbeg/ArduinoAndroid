#include <transfer.h>

Transfer::Transfer(String type, TransferData* data)
    :type(type), data(data)
{

}

void Transfer::toJson(JsonDocument& json, String& jsonString) {
    json["type"] = type;
    // json["message"] = getMessage();
    // data->addObjectToJson(json, jsonString);
    data->toJsonString("data", json, jsonString);

    serializeJson(json, jsonString);
}