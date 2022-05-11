/**
 * A-EPS029 - Universitat de les Illes Balears
 * Jonathan Salisbury Vega
 * 
 * Script to measure capacity with Bluetooth and 
 * send data to MQTT Broker using ESP32 microcontroller.
 */

 /* Includes */
#include <WiFi.h>
#include <PubSubClient.h>
#include <BluetoothSerial.h>
#include "credentials.h" // Wifi Credentials

/* Check if Bluetooth is Enabled */
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

/* Constants */
#define BT_DISCOVER_TIME  10000
#define IDLE_TIME 10000

/* Clients */
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
BluetoothSerial SerialBT;

/* Variables */
long lastMsg = 0;

void setup() {
  Serial.begin(115200);
  setupWifi(); // Connect to Wifi
  mqttClient.setServer(mqtt_server, 1883); // Connect to MQTT Server
  mqttClient.setCallback(callback);
  SerialBT.begin("ESP32test"); // Start Bluetooth
}

/* Method that connects this client to the Wifi Network */
void setupWifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

/* Callback method for receiving MQTT Server messages */ // NOT USED
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

/* Method that connects to Mqtt Server */
void connectToMQTTSever() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("connected");
      //client.subscribe("esp32/output"); Subscribe if needed
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

/* Method that Discovers Nearby devicex with BT
 *  and returns the number of devices found.
 */
int getBTDevices(){
  Serial.println("Discovering Devices...");
  BTScanResults *pResults = SerialBT.discover(BT_DISCOVER_TIME);
  if (pResults)
    pResults->dump(&Serial);
    return pResults->getCount();
  return -1;
}

/* Main Loop */
void loop() {
  if (!mqttClient.connected()) {
    connectToMQTTSever();
  }
  mqttClient.loop();
  long now = millis();
  if (now - lastMsg > IDLE_TIME) {
    lastMsg = now;
    int nDevices = getBTDevices();
    char cstr[16];
    itoa(nDevices, cstr, 10);
    if(nDevices != -1){
      Serial.println("Publishing info...");
      mqttClient.publish("aforos/", cstr);
    } else {
      Serial.println("Error on BT Scan, no result!");
    }
  }
}
