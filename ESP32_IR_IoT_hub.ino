/****************************************************************************
 IR ESP32 IoT hub

 Using an ESP 32 board as an IR receiver, to take IR remote control commands
 and respond by sending out MQTT messages.

 Requires the file wifi-credentials.h to be in the same location as this
 sketch. See the wifi-credentials-template.h file, which you can edit to
 suit your wifi environment.

 Use:
  Board:  ESP32 Dev Module
  Upload speed: 921600
  CPU freq: 240MHz
  Flash freq: 80MHz
  Flash mode: QIO
  Port: /dev/cu.SLAB_USBtoUART
 ****************************************************************************/

#include <WiFi.h>
#include "wifi-credentials.h"
#include <IRremote.h>
#include "irdecode.h"
#include <PubSubClient.h>


#define LED 33
#define RECV_PIN 25
#define SERVER_ERR_LIMIT 5
#define WIFI_MAX_TRIES 10
#define MQTT_SERVER_ADDR "10.0.0.59"
#define MQTT_TOPIC "home/lights"

// --- IR SENSOR --------------------------------------------------------------------------
// Define IR Receiver and Results Objects
IRrecv irrecv(RECV_PIN);
decode_results results;

// --- WIFI -------------------------------------------------------------------------------
int wifi_status = WL_IDLE_STATUS;
IPAddress ip;
uint8_t server_errors = 0;

void wifiConnect() {
  uint8_t ssid_idx = 0;
  uint8_t connect_counter = 0;
  WiFi.mode(WIFI_STA);
  while (connect_counter < WIFI_MAX_TRIES) {
    Serial.print("Attempting to connect to "); Serial.println(ssid[ssid_idx]);
    WiFi.begin(ssid[ssid_idx], WLAN_PASS);  // try to connect
    // delay to allow time for connection - flash lights to show this is happening
    flashLED(5, 100);
    wifi_status = WiFi.status();
    connect_counter++;
    if (wifi_status != WL_CONNECTED) {
      ssid_idx = 1 - ssid_idx;    // swap APs
    } else {
      Serial.println("Connected!");
      connect_counter = WIFI_MAX_TRIES; // to break out of the loop
      ip = WiFi.localIP();
      Serial.print("IP: "); Serial.println(ip);
      server_errors = 0;
    }
  }
  if (wifi_status != WL_CONNECTED) {  // wifi connection failed
    server_errors = SERVER_ERR_LIMIT;
  }
}

void flashLED(uint8_t times, int pulseLen) {
   for (uint8_t i = 0; i < times; i++) {
     digitalWrite(LED, HIGH);
     delay(pulseLen);
     digitalWrite(LED, LOW);
     delay(pulseLen);
   }
}

// --- MQTT -------------------------------------------------------------------------------
WiFiClient mqttWifiClient;
PubSubClient mqtt(mqttWifiClient);

/**
Callback function for incoming MQTT messages.
**/
void mqttCallback(char* topic, byte* message, unsigned int length) {
  // Topic is a char array. Easiest to cast it to a string to test
  // for specific topics.
  if (String(topic) == MQTT_TOPIC) {
    /* Just a placeholder to show how this works */
  }
  // The message is a byte array. We can build a string from it with:
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)message[i];
  }
}

void mqttReconnect() {
  // Loop until we're reconnected
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect("ESP32_IR_hub")) {
      Serial.println("MQTT connected");
      mqtt.subscribe("home/lights");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// ***************************************************************************************
// ***  SETUP                                                                          ***
// ***************************************************************************************
void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  // connect to wifi
  wifiConnect();

  // set up MQTT
  mqtt.setServer(MQTT_SERVER_ADDR, 1883);
  mqtt.setCallback(mqttCallback);
  
  // Enable the IR Receiver
  irrecv.enableIRIn();
  Serial.println("Listening for signals...");
}

// ***************************************************************************************
// ***  LOOP                                                                           ***
// ***************************************************************************************

void loop() {
  if (!mqtt.connected()) {
    mqttReconnect();
  }
  mqtt.loop();
  
  if (irrecv.decode(&results)) {
    if(results.value != 0xFFFFFFFF) {
      char resultsBuf[40];
      flashLED(2, 10);
      decodeDetails(resultsBuf, results);
      Serial.println(resultsBuf);
    }
    
    // Send MQTT message according to code received
    switch (results.value) {
      case 0x807FC03F:
        Serial.println("- sending dim message");
        mqtt.publish(MQTT_TOPIC, "dim");
        break;
    }
    
    irrecv.resume();
  } // if irrecv.decode
}
