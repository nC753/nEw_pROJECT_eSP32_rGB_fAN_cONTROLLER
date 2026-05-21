ESP32 Development Board (38-pin DevKit V1): The "brain" that handles Wi-Fi, MQTT, PWM motor control, and LED signals.
Breadboard & Jumper Wires: Necessary for prototyping. Use Male-to-Male and Male-to-Female wires.
Logic Level Shifter (Optional but Recommended): The ESP32 outputs 3.3V signals, but your 12V fan's PWM pin prefers a higher voltage signal. A level shifter ensures the fan receives the correct signal to spin at the requested speed.
12V DC Power Supply: A 12V supply is required to power the fan. Do not power the fan from the ESP32's 3.3V or 5V pins, as this will fry your board.
DS18B20 Temperature Sensor: The digital probe that monitors your fan's environment.
4.7k Ohm Resistor: CRITICAL. This is a pull-up resistor required by the DS18B20 to transmit data accurately.
10k Ohm Resistor: Needed for the fan's tachometer (RPM) wire to ensure the ESP32 can "see" the pulses clearly without noise interfering.
Fan ARGB Cooler
