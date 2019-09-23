/****************************************************************************
 IR ESP32 IoT hub

 Using an ESP 32 board as an IR receiver, to take IR remote control commands
 and respond by sending out MQTT messages.

 Use:
  Board:  ESP32 Dev Module
  Upload speed: 921600
  CPU freq: 240MHz
  Flash freq: 80MHz
  Flash mode: QIO
  Port: /dev/cu.SLAB_USBtoUART
 ****************************************************************************/

#include <WiFi.h>
#include <IRremote.h>

#define LED 33
#define RECV_PIN 25

// --- IR SENSOR --------------------------------------------------------------------------
// Define IR Receiver and Results Objects
IRrecv irrecv(RECV_PIN);
decode_results results;
 
// --- WIFI -------------------------------------------------------------------------------
#define SERVER_ERR_LIMIT 5
#define WLAN_PASS      "CezanneZola"
#define WIFI_MAX_TRIES 12
const char* ssid [] = {"Montcocher", "Montcocher-Livebox"};
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
    for (uint8_t i = 0; i < 5; i++) {
      digitalWrite(LED, HIGH);
      delay(150);
      digitalWrite(LED, LOW);
      delay(150);
    }
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

// ***************************************************************************************
// ***  SETUP                                                                          ***
// ***************************************************************************************
void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
//  for(uint8_t i=0; i<5; i++) {
//    digitalWrite(LED, HIGH);
//    delay(250);
//    digitalWrite(LED, LOW);
//    delay(250);
//  }
  wifiConnect();
  // Enable the IR Receiver
  irrecv.enableIRIn();
  Serial.println("Listening for signals...");
}

// ***************************************************************************************
// ***  LOOP                                                                           ***
// ***************************************************************************************

void loop() {
  if (irrecv.decode(&results)) {
    if(results.value != 0xFFFFFFFF) {
      Serial.print("0x");
      Serial.print(results.value, HEX);
      Serial.print("  ");
      Serial.print(results.value, HEX);
      Serial.print("  ");
      Serial.print(results.bits);
      Serial.print("  ");
      switch (results.decode_type){
        // --- most likely ---------------
        case NEC: 
          Serial.println("NEC"); 
          break;
        case SONY: 
          Serial.println("SONY"); 
          break;
        case RC5: 
          Serial.println("RC5"); 
          break;
        case RC6: 
          Serial.println("RC6"); 
          break;
        // --- others --------------------
        case AIWA_RC_T501: 
          Serial.println("AIWA_RC_T501"); 
          break;
        case DENON: 
          Serial.println("DENON"); 
          break;
        case DISH: 
          Serial.println("DISH"); 
          break;
        case JVC: 
          Serial.println("JVC"); 
          break;
        case LG: 
          Serial.println("LG"); 
          break;
        case MITSUBISHI: 
          Serial.println("MITSUBISHI"); 
          break;
        case PANASONIC: 
          Serial.println("PANASONIC"); 
          break;
        case SAMSUNG: 
          Serial.println("SAMSUNG"); 
          break;
        case SANYO: 
          Serial.println("SANYO"); 
          break;
        case SHARP: 
          Serial.println("SHARP"); 
          break;
        case WHYNTER: 
          Serial.println("WHYNTER"); 
          break;
        case UNKNOWN:
          Serial.println("UNKNOWN"); 
          break;
        default:
          Serial.println("No idea"); 
          break;
      } // switch
    } // if results.value
    irrecv.resume();
  } // if irrecv.decode
}
