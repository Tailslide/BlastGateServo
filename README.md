# BlastGateServo
Blast gate servo controller for Arduino

See 3d printed parts at : https://www.thingiverse.com/thing:3301904

## Features
* Allows a single push button to switch between different blast gates, with only a single one open at a time
* Currently open blast gate is indicated by an LED
* Optional automatic control using AC current sensors to detect active tools
* Debug output support for troubleshooting
* Configurable servo positions for precise gate control

## Hardware Requirements
* Arduino Uno (R3 or R4)
* A servo for each blast gate (up to 4)
* One push button
* Optional: An LED for each blast gate
* Optional: AC current sensors from http://moderndevice.com/current

## Development Setup
1. Install Visual Studio Code
2. Install PlatformIO IDE extension for VSCode
3. Open this project folder in VSCode
4. PlatformIO will automatically install required dependencies:
   - Servo library (v1.2.1 or later)

## Building and Uploading
1. Connect your Arduino Uno to your computer via USB
2. In VSCode:
   - Click the PlatformIO icon in the sidebar
   - Under Project Tasks:
     - Click "Build" to compile
     - Click "Upload" to flash to Arduino
   - Use "Monitor" to view serial output (when debugging is enabled)
3. Serial monitor is configured at 9600 baud

## Operation Modes
* Normal Mode: Use the push button to cycle through gates. After selecting a gate, wait briefly and it will open automatically.
* Meter Mode: For calibrating AC sensors. Enter this mode by holding the button while powering up the Arduino.
* Debug Mode: Enable detailed serial output by setting DEBUG flag (enabled by default)

## Configuration
All settings can be adjusted in Configuration.h:

### Basic Settings
* Number of gates (NUM_GATES) - up to 4 supported
* Number of AC sensors (NUM_AC_SENSORS) - typically one per gate
* Number of LEDs (NUM_LEDS) - typically one per gate
* Button pin assignment (BUTTON_PIN)

### Timing Settings
* CLOSE_DELAY - Time it takes for a gate to close (ms)
* OPEN_DELAY - Time after last button push to open gate (ms)
* MAX_BLINK_LEN - LED blink rate (adjusted automatically in debug mode)

### AC Sensor Settings
* NUM_OFF_SAMPLES - Number of samples for checking average sensor off values
* NUM_OFF_MAX_SAMPLES - Milliseconds to sample for max off value at startup
* AVG_READINGS - Number of readings to average when triggering gates (max 50)
* AC_SENSOR_SENSITIVITY - Trigger threshold multiplier (2.0 = twice max off reading)

### Pin Assignments
* Servo pins (SERVO_PIN_1 through SERVO_PIN_4)
* AC sensor pins (AC_SENSOR_PIN_1 through AC_SENSOR_PIN_4)
* LED pins (LED_PIN_1 through LED_PIN_4)

### Servo Position Configuration
* SERVO_MAX_x - Closed position for each servo (0-180 degrees)
* SERVO_MIN_x - Open position for each servo (0-180 degrees)

## Project Structure
* BlastGateServo.ino - Main program file with setup and loop
* Configuration.h - All user configurable settings
* GateServos.h/cpp - Servo control and position management
* AcSensors.h/cpp - AC current sensor reading and threshold detection
* Debug.h - Debug output macros and configuration
* platformio.ini - PlatformIO project configuration and library dependencies

Created 2019-01-02 - Greg Pringle
Updated 2019-01-20 - Greg Pringle - Added AC sensor support
Updated 2025-01-26 - Added VSCode/PlatformIO support
Updated 2025-02-16 - Added detailed configuration documentation
