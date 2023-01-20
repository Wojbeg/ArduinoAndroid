#include <exception>

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <ArduinoJson.h>
#include "message.h"
#include "transfer.h"

using Byte = unsigned char;

constexpr int SERIAL_SPEED = 115200;

constexpr int DHTPIN = 2;

constexpr int RED_LED = 5;
constexpr int GREEN_LED = 4;
constexpr int BLUE_LED = 0;

#define DHTTYPE DHT11

enum class State{
  st_0,
  st_1_cmd,
  st_2_length,
  st_3_data
};

constexpr int ST_0 = 0;      //waiting Sync word
constexpr int ST_1_CMD = 1;  //Waiting CMD
constexpr int ST_2_LENGTH= 2;//Receiving Length for CMD_03_TEXT
constexpr int ST_3_DATA= 3;  //Receiving Data for CMD_03_TEXT
// const byte IGNORE_00 = 0x00;

// const byte SYNC_WORD = 0xFF;

// const byte CMD_01_LEDON = 0x01;
// const byte CMD_02_LEDOFF= 0x02;
// const byte CMD_03_TEXT  = 0x03;

constexpr Byte SYNC_WORD = static_cast<Byte>(0x00);
constexpr Byte CMD_03_TEXT  = static_cast<Byte>(0x03);
constexpr Byte CMD_02_START_OF_TEXT = static_cast<Byte>(0x02);
constexpr Byte CMD_05_LEDON = static_cast<Byte>(0x05);
constexpr Byte CMD_06_LEDOFF= static_cast<Byte>(0x06);

State cmdState;
int dataIndex;

constexpr int MAX_LENGTH = 1024;
std::array<Byte, MAX_LENGTH> data;
int dataLength;


const unsigned long two_seconds = 2000;

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;


const char* ssid = "test";
const char* password = "password";

ESP8266WebServer server(80);
String myIP;

bool isLedWorking = true;
float temperature = 0;
float relativeHumadity = 0;
int redValue = 0;
int greenValue = 0;
int blueValue = 0;

String buffer;  // string (at least std string) is empty by default; no need to assign empty string to it
// String textBuffer = "";


StaticJsonDocument<MAX_LENGTH> doc;
String jsonData;

/////////////

void setupLED() {
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
}

void setupDHTSensor() {
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

void setupWifi() {

  // WiFi.begin(ssid, password);
  // Serial.print("Connecting to WiFi");
  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   delay(500);
  //   Serial.print(".");
  // }
  // Serial.println();
  // Serial.print("Connected, IP address: ");
  // Serial.println(WiFi.localIP());

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  myIP = WiFi.softAPIP().toString();

  Serial.println("AP ip address is: ");
  Serial.println(myIP);
  Serial.println("\n");
}


String SendHTML() {
  String ptr = "<!DOCTYPE html>\n";
  ptr +="<html>\n";
  ptr +="<head>\n";
  ptr +="<title>Stacja pogodowa v0.0.1</title>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";

  ptr +="<h1>Wartosci:</h1>\n";
  ptr += buffer;
  ptr += "\n";

  // ptr +="<h1>Wartosci 2:</h1>\n";
  // ptr += textBuffer;
  // ptr += "\n";

  ptr +="<h1>Temperatura:</h1>\n";
  ptr += String(temperature);
  ptr += '\370';
  ptr +="\n<h1>Wilgotnosc:</h1>\n";
  ptr += relativeHumadity;
  ptr += "\n";
  ptr +="<h1>LED</h1>\n";
  ptr +="<p>Kliknij, żeby wlaczyc lub wylaczyc diodę LED.</p>\n";
  ptr +="<form method=\"get\">\n";
  if(isLedWorking)
  ptr +="<input type=\"button\" value=\"LED OFF\" onclick=\"window.location.href='/ledoff'\">\n";
  else
  ptr +="<input type=\"button\" value=\"LED ON\" onclick=\"window.location.href='/ledon'\">\n";
  ptr +="</form>\n";
  ptr += "<h1>Czerwone: " + String(redValue) + "</h1>\n";
  ptr +="<form method=\"get\">\n";
  ptr += "<input id='red' type='number' name='red' value='" + String(redValue) + "' min='0' max='255' />";
  ptr += "<input type='submit'>";
  ptr +="</form>\n";
  ptr += "<h1>Zielone: " + String(greenValue) + "</h1>\n";
  ptr +="<form method=\"get\">\n";
  ptr += "<input id='green' type='number' name='green' value='" + String(greenValue) + "' min='0' max='255' />";
  ptr += "<input type='submit'>";
  ptr +="</form>\n";
  ptr += "<h1>Niebieskie: " + String(blueValue) + "</h1>\n";
  ptr +="<form method=\"get\">\n";
  ptr += "<input id='blue' type='number' name='blue' value='" + String(blueValue) + "' min='0' max='255' />";
  ptr += "<input type='submit'>";
  ptr +="</form>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}


[[nodiscard]] int returnValidColor(int newValue) {
  if (newValue < 0) {
    return 0;
  } else if (newValue > 255) {
    return 255;
  }

  return newValue;
}

void checkArgs() {

  for (int i = 0; i < server.args(); i++)
  {
    if (server.argName(i) == "red") {
      redValue = returnValidColor( server.arg(i).toInt() );
    } else if(server.argName(i) == "green") {
      greenValue = returnValidColor( server.arg(i).toInt() );
    } else if(server.argName(i) == "blue") {
      blueValue = returnValidColor( server.arg(i).toInt() );
    } else {
      Serial.println("Unknown argument: " + server.arg(i));
    }

  }

}

void handleJson() {

  // doc.clear();

  DeserializationError error = deserializeJson(doc, data);

  if (error) {
    Serial.write("{\"type\":\"error\",\"data\":{\"error\":\"Error parsing JSON\"}}");
    Serial.write("\n");
    return;
  }

  if(doc.containsKey("type") && doc.containsKey("data")) {

    if(doc["type"] == "set") {
      JsonObject data = doc["data"];

      if(data.containsKey("colors")) {
        redValue = data["colors"][0];
        greenValue = data["colors"][1];
        blueValue = data["colors"][2];
      }


      if(data.containsKey("text")) {
        buffer.concat(data["text"].as<String>());
        buffer.concat("\n");
      }
    }

  }

}

void cmdHandle(Byte incomingByte) {

  if (incomingByte == SYNC_WORD || incomingByte == CMD_02_START_OF_TEXT) {
    cmdState = State::st_3_data;
    // buffer.concat("s ");
    return;
  }

  // if(incomingByte == IGNORE_00){
  //   buffer.concat("i ");
  //   return;
  // }

  // char character = char (incomingByte);
  // buffer.concat(&character);

  switch (cmdState)
  {
    case State::st_1_cmd:
      switch (incomingByte)
      {
        case CMD_05_LEDON:
          isLedWorking = true;
          // buffer.concat("L_on ");
          break;
        case CMD_06_LEDOFF:
          // buffer.concat("L_off ");
          isLedWorking = false;
          break;
        case CMD_03_TEXT:
          // buffer.concat("tx ");
          data.fill('\0');

          cmdState = State::st_2_length;
          dataIndex = 0;
          dataLength = 0;
          break;
        default:
          cmdState = State::st_0;
      }
    break;

    case State::st_2_length:
      // buffer.concat("le ");

      if(incomingByte < 0) {      // may our byte be negative? If so, we need to change definition of type Byte to signed char. On the other hand, in such a case
        dataLength = 256 + incomingByte;  // Byte would handle values from -127 to 128 only
      } else {
        dataLength += incomingByte;
      }

      if(dataLength > MAX_LENGTH){
        dataLength = MAX_LENGTH;
      }

      break;

    case State::st_3_data:
      // buffer.concat("d ");

      data[dataIndex] = incomingByte;
      dataIndex++;
      // buffer.concat(incomingByte);

      if(dataIndex == dataLength){
        cmdState = State::st_0;

        // buffer = data;

        // Serial.write(buffer.c_str(), buffer.length());
        // Serial.write("\n");
        // from char to parse json

        handleJson();
      }
      break;

    default:
      Serial.println("UNEXCPECTED STATE IN CMDSTATE");

  }

}

////////////////////////////////////////////

void handleOnConnect() {
  checkArgs();
  server.send(200, "text/html", SendHTML());
}

void handleLedOn() {
  isLedWorking = true;
  checkArgs();
  server.send(200, "text/html", SendHTML());
}

void handleLedOff() {
  isLedWorking = false;
  checkArgs();
  server.send(200, "text/html", SendHTML());
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void setupServer() {
  server.on("/", handleOnConnect);
  server.on("/ledon", handleLedOn);
  server.on("/ledoff", handleLedOff);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void updateLed() {
  if(isLedWorking) {
    digitalWrite(LED_BUILTIN, LOW);
    analogWrite(RED_LED, redValue);
    analogWrite(GREEN_LED, greenValue);
    analogWrite(BLUE_LED, blueValue);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
  }
}

void updateDhtSensor() {

  static unsigned long last_reading_time = 0UL;

  if (millis() - last_reading_time >= delayMS)
  {
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      // Serial.println(F("Error reading temperature!"));
      // Serial.println(F("Old reading used:"));
      temperature = random(-10, 30);
    }
    else {
      temperature = event.temperature;
    }


    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      // Serial.println(F("Error reading humidity!"));
      // Serial.println(F("Old reading used:"));
      relativeHumadity = random(0, 100);
    }
    else {
      relativeHumadity = event.relative_humidity;
    }


    last_reading_time = millis();
  }

}


void printSerial() {
  static unsigned long last_serial_update = 0UL;

  if(millis() - last_serial_update >= two_seconds) {  // we should use std::chrono here but it would be harder to use so I'm ok with millis but consider it please

    // Serial.write("Temperature: ");

    // char destination[9];
    // dtostrf(temperature, 6, 2, destination);
    // Serial.write(destination);
    // Serial.write("°C\n");

    // Serial.write("Humidity: ");

    // dtostrf(relativeHumadity, 6, 2, destination);
    // Serial.write(destination);
    // Serial.write("%\n");

    // Serial.write("Colors:(");
    // Serial.write(itoa(redValue, destination, 10));
    // Serial.write(",");
    // Serial.write(itoa(greenValue, destination, 10));
    // Serial.write(",");
    // Serial.write(itoa(blueValue, destination, 10));
    // Serial.write(")\n");

    jsonData.clear();
    Message message(temperature, relativeHumadity, isLedWorking, myIP, {redValue, greenValue, blueValue});
    Transfer transfer("info", &message);

    transfer.toJson(doc, jsonData);

    Serial.write(jsonData.c_str(), jsonData.length());
    Serial.write("\n");

    last_serial_update = millis();
  }
}

/////////////


void setup() {
  Serial.begin(SERIAL_SPEED);
  setupDHTSensor();
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  setupLED();
  randomSeed(analogRead(0));
  setupWifi();
  setupServer();
}


void loop() {
  server.handleClient();

  updateLed();
  updateDhtSensor();

  if(Serial.available()) {

    while (Serial.available() > 0) {


    Byte data = Serial.read();

    cmdHandle(data);
    // textBuffer.concat(data);

    // int decide = random(3);
    // int color = random(255);
    // if (decide == 0){
    //   redValue = color;
    // } else if (decide == 1) {
    //   greenValue = color;
    // } else {
    //   blueValue = color;
    // }

    }

  }

  printSerial();

}