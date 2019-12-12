/*
 * DHT22 Temperature Sensor
 * 
 */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include "wifi-credentials.h"
#define DHTPIN D2
#define DHTTYPE DHT22

const char *ssid = WIFISSID;
const char *password = WIFIPASSWORD;

ESP8266WebServer server(80);

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);
 
void setup() {
  //initialize Serial console
  Serial.begin(9600);
  delay(100);

  //initialize DHT22 sensor
  dht.begin();              

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

  //initialize web server paths, then start the server
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

//returns the current temperature and humidity in json format.
void handle_state() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  String message = "";
  Serial.print("Temperature in Celsius : ");
  Serial.println(temperature);
  Serial.print("Relative Humidity : ");
  Serial.println(humidity);
  Serial.println();
  message += "{";
  message += "\"temperature\": ";
  message += temperature;
  message += ", ";
  message += "\"humidity\": ";
  message += humidity;
  message += " }";
  server.send(200, "application/json", message);
}

//return a 404 page
void handle_notFound(){
  server.send(404, "text/plain", "Not found");
}
