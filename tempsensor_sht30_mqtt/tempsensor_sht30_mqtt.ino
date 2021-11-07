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
#include <PubSubClient.h>

#include "wifi-credentials.h"

const char *ssid = WIFISSID;
const char *password = WIFIPASSWORD;

#define mqtt_server "10.10.10.1"
#define temperature_topic "sensor/livingroom/temperature"
#define humidity_topic "sensor/livingroom/humidity"

long lastMsg = 0;
bool debug = true;

SHT3X sht30(0x44);
WiFiClient espClient;
PubSubClient client(espClient);

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

  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  // Send a message every minute
  if (now - lastMsg > 1000 * 60) {
    lastMsg = now;
    // Read data
    Serial.println("Reading data");
    if(sht30.get() == 0) {
      float h = sht30.humidity;
      float t = sht30.cTemp;
      if ( isnan(t) || isnan(h)) {
        Serial.println("KO, Please check sensor !");
        return;
      }
      if ( debug ) {
        Serial.print("Temperature : ");
        Serial.print(t);
        Serial.print(" | Humidity : ");
        Serial.println(h);
      }
      client.publish(temperature_topic, String(t).c_str(), true);
      client.publish(humidity_topic, String(h).c_str(), true);
    }
    else {
      Serial.println("KO, Please check sensor!");
      return;
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ...");
    delay(1000);
    if (client.connect("ESP8266Client3")) {
      Serial.println("OK");
    } else {
      Serial.print("KO, error : ");
      Serial.print(client.state());
      Serial.println(" Wait 5 secondes before to retry");
      delay(5000);
    }
  }
}
