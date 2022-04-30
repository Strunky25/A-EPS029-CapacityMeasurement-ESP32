/**
 * A simple Azure IoT example for sending telemetry.
 */

#include <WiFi.h>
//#include "Esp32MQTTClient.h"
#include <PubSubClient.h>

// Please input the SSID and password of WiFi
const char* ssid     = "iotuib";
const char* password = "dvSGnJc32940k23fASF9";

const char* mqtt_server = "172.22.39.7";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
      //client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

int getBTDevices(){
  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  /*
  if (Serial.available() > 0) {
    int bufLen = 20;
    char incomingByte[bufLen];
    Serial.readString().toCharArray(incomingByte, bufLen);

    // say what you got:
    Serial.print("Sending: ");
    Serial.println(incomingByte);
    client.publish("aforos/prueba", incomingByte);
  } */

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    client.publish("aforos/", "prueba15");
  }
}
