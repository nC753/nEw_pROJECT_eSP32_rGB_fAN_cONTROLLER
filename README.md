## 🛠️ Hardware Requirements (Bill of Materials)

To build this project, you will need the following components. 

### 🧠 Core Controller & Prototyping
* **ESP32 Development Board (38-pin DevKit V1):** The "brain" of the project that handles Wi-Fi connectivity, MQTT communication, PWM motor control, and LED data signals.
* **Breadboard & Jumper Wires:** Necessary for prototyping the circuit. You will need a mix of Male-to-Male and Male-to-Female jumper wires.

### ⚡ Power & Cooling
* **Fan ARGB Cooler (WS2812B) [3x4-pin]:** The main PC cooling fan featuring Addressable RGB LEDs for custom lighting effects.
* **12V DC Power Supply:** A dedicated 12V power supply is required to provide enough muscle to spin the fan.
> ⚠️ **CRITICAL WARNING:** Do **not** attempt to power the 12V fan directly from the ESP32's 3.3V or 5V pins. Doing so will instantly fry your microcontroller!

### 🌡️ Sensors & Signal Control
* **DS18B20 Temperature Sensor:** A digital temperature probe used to monitor the fan's physical environment.
* **Logic Level Shifter** *(Optional but Recommended)*: The ESP32 outputs 3.3V signals, but the 12V fan's PWM pin prefers a higher voltage signal. A logic level shifter ensures the fan receives the correct, strong signal to spin at the requested speed.

### 🔌 Passive Components
* **4.7kΩ Resistor:** **CRITICAL.** This acts as a pull-up resistor required by the DS18B20 temperature sensor to transmit data accurately to the ESP32.
* **10kΩ Resistor:** Required for the fan's tachometer (RPM) wire. This ensures the ESP32 can cleanly "see" the RPM pulses without electrical noise interfering.
