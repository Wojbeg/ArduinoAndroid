#ifndef TRANSFER_H
#define TRANSFER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <transferData.h>

class Transfer
{
  public:
    Transfer(String type, TransferData* data);
    String getType();

    void toJson(JsonDocument& json, String& jsonString);

  private:
    String type;
    TransferData* data;
};

#endif