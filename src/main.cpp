#include "Arduino.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// --- FASTLED & TEMP SENSOR LIBRARIES ---
#include <FastLED.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// --- Fan Controller Hardware Pins ---
#define LED_DATA_PIN 26      
#define NUM_LEDS 120         
#define FAN_PWM_PIN 15       
#define TEMP_PIN 4           
#define PWM_CHANNEL 0        
#define PWM_FREQ 25000       
#define PWM_RESOLUTION 8     

// --- Network & MQTT Credentials ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_broker = "mqtt.iotserver.uz";  
const int mqtt_port = 1883;
const char* mqtt_username = "userTTPU";  
const char* mqtt_password = "mqttpass";  

// --- MQTT Topics ---
const char* topic_fan_set  = "ttpu/iot/kamronbek/fan/set";

// --- Global State Variables ---
bool acceptCommands = true;  
int currentFanSpeed = 0;     
int currentBrightness = 100; 
CRGB leds[NUM_LEDS]; 

// --- Global Objects ---
WiFiClient espClient;
PubSubClient mqtt_client(espClient);
OneWire oneWire(TEMP_PIN);
DallasTemperature tempSensor(&oneWire);

// --- Function Declarations ---
void connectWiFi();
void connectMQTT();
void MQTT_callback(char* topic, byte* payload, unsigned int length);

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  Serial.println("Booting System...");

  // 1. Initialize FastLED Lights
  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(map(currentBrightness, 1, 100, 0, 255));
  FastLED.clear(); 
  FastLED.show();

  // 2. Initialize Fan Motors (ledc)
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(FAN_PWM_PIN, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, 0); 

  // 3. Initialize Temperature Sensor
  tempSensor.begin();

  // 4. Connect WiFi (Using your upgraded function)
  connectWiFi();

  // 5. Connect MQTT (Using your upgraded function)
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setCallback(MQTT_callback);
  connectMQTT();

  Serial.println("System Ready!");
}

// ==========================================
// LOOP
// ==========================================
void loop() {
  // Check and maintain connections (From your template)
  if (WiFi.status() != WL_CONNECTED) { connectWiFi(); }
  if (!mqtt_client.connected()) { connectMQTT(); }
  mqtt_client.loop();
}

// ==========================================
// FUNCTIONS
// ==========================================

void MQTT_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("\n[MQTT] Received on ");
  Serial.print(topic);
  Serial.print(": ");
  
  // Convert payload to readable string
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }
  Serial.println(messageTemp);

  // --- Handle Malformed JSON safely ---
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, messageTemp);
  if (error) {
    Serial.print("Failed to parse JSON. Error: ");
    Serial.println(error.c_str());
    return;
  }

  // --- Handle RGB Fan & Speed Messages ---
  if (strcmp(topic, topic_fan_set) == 0) {
    if (!acceptCommands) {
      Serial.println("System is locked. Ignoring command.");
      return;
    }

    // Check for Brightness Update
    if (doc["brightness"]) {
      currentBrightness = doc["brightness"]; 
      FastLED.setBrightness(map(currentBrightness, 1, 100, 0, 255));
      Serial.print("[LED] Brightness updated to: ");
      Serial.println(currentBrightness);
    }

    // Check for Fan Motor Speed Update
    if (doc["speed"]) {
      currentFanSpeed = doc["speed"];
      ledcWrite(PWM_CHANNEL, currentFanSpeed);
      Serial.print("[MOTOR] Fan speed updated to: ");
      Serial.println(currentFanSpeed);
    }

    // Check for Hex Color Update
    if (doc["color"]) {
      const char* hexColor = doc["color"];
      CRGB newColor = strtol(hexColor + 1, NULL, 16); 
      fill_solid(leds, NUM_LEDS, newColor);
      Serial.print("[LED] Color updated to: ");
      Serial.println(hexColor);
    }

    // Push physical updates to the LEDs
    FastLED.show();
  }
}

// --- YOUR UPGRADED WIFI FUNCTION ---
void connectWiFi() {
  Serial.print("\nConnecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nWiFi connected! IP Address: ");
  Serial.println(WiFi.localIP());
}

// --- YOUR UPGRADED MQTT FUNCTION ---
void connectMQTT() {
  while (!mqtt_client.connected()) {
    Serial.print("Connecting to MQTT broker... ");
    
    // Create unique client ID based on MAC Address
    String client_id = "ESP32_Kamronbek_" + String(WiFi.macAddress());
    
    // Connect using username and password
    if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected!");
      
      // Subscribe to the Fan command topic
      mqtt_client.subscribe(topic_fan_set);
      
      Serial.println("Subscribed to Fan command topic.");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(". Retrying in 5 seconds...");
      delay(5000);
    }
  }
}