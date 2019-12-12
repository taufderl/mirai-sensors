/*
 * SHT30 Temperature Sensor
 * 
 * https://github.com/wemos/WEMOS_SHT3x_Arduino_Library
 * Pins:
 *  3.3V - VIN
 *  G - GND
 *  D1 - SCL
 *  D2 - SAA
 */
#include <WEMOS_SHT3X.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "wifi-credentials.h"

const char *ssid = WIFISSID;
const char *password = WIFIPASSWORD;

ESP8266WebServer server(80);
SHT3X sht30(0x44);

void setup() {
  Serial.begin(9800);
  delay(100);

  //initialize WiFi connection
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());

  //initialize web server
  server.on("/", handle_root);
  server.on("/state", handle_state);
  server.onNotFound(handle_notFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
//the root directory redirects to /state
void handle_root() {
  server.sendHeader("Location","/state");
  server.send(301, "text/plain", "");
}


void handle_state() {
  Serial.println("Retrieving sensor data...");
  String message = "";
  if(sht30.get() == 0) {
    Serial.print("Temperature in Celsius : ");
    Serial.println(sht30.cTemp);
    Serial.print("Relative Humidity : ");
    Serial.println(sht30.humidity);
    Serial.println();
    message += "{";
    message += "\"temperature\": ";
    message += sht30.cTemp;
    message += ", ";
    message += "\"humidity\": ";
    message += sht30.humidity;
    message += " }";
  } else {
    Serial.println("Error!");
    message = "{\"error\": \"Could not retreive data from sensor.\"}";
  }
  server.send(200, "application/json", message);
}

//return a 404 page
void handle_notFound(){
  server.send(404, "text/plain", "Not found");
}
