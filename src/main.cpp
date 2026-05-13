#include "Arduino.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>
#include <TaskScheduler.h>

// --- NEW TEMPERATURE LIBRARIES ---
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "192.168.1.100"; // Replace with your MQTT broker IP
const char* topic_subscribe = "pc/fans/set";
const char* topic_publish = "pc/fans/status";

// --- HARDWARE PINS & CONFIG ---
// You just define the single data pin, plus a definition for the total number of LEDs[cite: 174].
#define LED_DATA_PIN 26      
#define NUM_LEDS 120         // Adjust this to your total (e.g., 4 fans x 30 LEDs)
#define FAN_PWM_PIN 15       // Pin that sends the 25kHz speed limit signal
#define FAN_TACH_PIN 32      // Pin that reads the RPM speed
#define PWM_CHANNEL 0        // The internal ledc channel to use
#define PWM_FREQ 25000       // PC fans require a 25kHz PWM signal
#define PWM_RESOLUTION 8     // 8-bit resolution (0-255 speed values)

// --- GLOBAL STATE VARIABLES ---
bool acceptCommands = true;  // The virtual lock switch
int currentFanSpeed = 0;     // Stores the current PWM speed limit
int currentRPM = 0;          // Stores the actual calculated RPM
int currentBrightness = 100; // Brightness percentage (1-100%)

// FastLED Array (Holds the color data for every individual LED)
CRGB leds[NUM_LEDS]; 

// --- LIBRARY INSTANCES ---
WiFiClient espClient;
PubSubClient client(espClient);
Scheduler taskRunner; // Your TaskScheduler instance

void setup() {
  // Start the serial monitor so you can read debug messages on your PC
  Serial.begin(115200);
  Serial.println("System Booting: Initializing LEDs...");

  // Tell FastLED your chip type (WS2812B is standard for ARGB fans), the pin, and the color order
  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
  
  // Set the master brightness limit
  // FastLED expects a 0-255 scale, so we map your 1-100% percentage to that scale
  FastLED.setBrightness(map(currentBrightness, 1, 100, 1, 255));

  // Clear any random colors from memory and push the black/off state to the physical fans
  FastLED.clear();
  FastLED.show();
  
  Serial.println("LEDs Initialized and Cleared!");
}

void loop() {
  // A simple test loop: Turn the very first LED on the first fan Red
  leds[0] = CRGB::Red; 
  FastLED.show();
  delay(1000);

  // Turn it to Black (Off)
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(1000);
}