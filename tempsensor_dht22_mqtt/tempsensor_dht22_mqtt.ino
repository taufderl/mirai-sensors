/*
 * DHT22 Temperature Sensor
 * 
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include "wifi-credentials.h"
#define DHTPIN D2
#define DHTTYPE DHT22

const char *ssid = WIFISSID;
const char *password = WIFIPASSWORD;

#define mqtt_server "10.10.10.1"
#define temperature_topic "sensor/serverroom/temperature"
#define humidity_topic "sensor/serverroom/humidity"

long lastMsg = 0;
bool debug = false;

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

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
    // Read humidity
    float h = dht.readHumidity();
    // Read temperature in Celcius
    float t = dht.readTemperature();

    // Oh, nothing to send
    if ( isnan(t) || isnan(h)) {
      Serial.println("KO, Please check DHT sensor !");
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
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect("ESP8266Client2")) {
      Serial.println("OK");
    } else {
      Serial.print("KO, error : ");
      Serial.print(client.state());
      Serial.println(" Wait 5 secondes before to retry");
      delay(5000);
    }
  }
}
