#ifndef TRANSFER_DATA_H
#define TRANSFER_DATA_H

#include <Arduino.h>
#include <ArduinoJson.h>

class TransferData
{
    public:
        virtual void addObjectToJson(JsonDocument& json, String& jsonString);
        virtual void toJsonString(String name, JsonDocument& json, String& jsonString);
};

#endif