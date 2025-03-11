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
* A servo for each blast gate (up to 5)
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
* Meter Mode: For calibrating AC sensors:
  1. Power off the Arduino
  2. Press and hold the button
  3. While holding the button, power on the Arduino
  4. Release the button after power-up
  5. LEDs will now act as signal strength indicators:
     - Each LED corresponds to an AC sensor
     - Turn on a device on the cable being calibrated
     - Rotate the sensor clamp around the cable
     - LED will blink faster (or become solid) when sensor is optimally positioned
* Debug Mode: Enable detailed serial output by setting DEBUG flag (enabled by default)
* LED Test Mode: Enable by uncommenting DEBUG_LED_TEST in Configuration.h. Flashes each LED in sequence to verify connections.
* Servo Test Mode: Enable by uncommenting DEBUG_SERVO_TEST in Configuration.h. Opens and closes a specified servo with each button press without initializing other components. Set TEST_SERVO_INDEX in Configuration.h to select which servo to test (1 = first servo, 2 = second servo, etc.). Useful for testing servo functionality and calibration. The system will properly handle disabled servos (pins set to -1) by controlling only the LED while skipping servo movement.

## Configuration
All settings can be adjusted in Configuration.h:

### Basic Settings
* Number of gates (NUM_GATES) - up to 5 supported
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
* Servo pins (SERVO_PIN_1 through SERVO_PIN_5)
  * Set any servo pin to -1 to disable that servo while maintaining the gate numbering
  * Example: Setting SERVO_PIN_2 to -1 disables the second servo but keeps gate 3 as "gate 3" in the UI
* AC sensor pins (AC_SENSOR_PIN_1 through AC_SENSOR_PIN_5)
* LED pins (LED_PIN_1 through LED_PIN_5)

### Servo Position Configuration
* SERVO_MAX_x - Maximum position for each servo (typically 180 degrees)
* SERVO_MIN_x - Minimum position for each servo (typically 0 degrees)
* GATE_CLOSED_AT_MAX_x - Determines gate orientation:
  * Set to true (default) if gate is closed when servo is at max position
  * Set to false if gate is open when servo is at max position (inverted)
* Set any servo's MAX/MIN values even if the pin is disabled (-1) to maintain consistent configuration

## Project Structure
* src/BlastGateServo.cpp - Main program file with setup and loop
* include/Configuration.h - All user configurable settings
* include/GateServos.h/cpp - Servo control and position management
* include/AcSensors.h/cpp - AC current sensor reading and threshold detection
* include/Debug.h - Debug output macros and configuration
* platformio.ini - PlatformIO project configuration and library dependencies

*Created 2019-01-02 - Greg Pringle
*Updated 2019-01-20 - Greg Pringle - Added AC sensor support
*Updated 2025-01-26 - Added VSCode/PlatformIO support
*Updated 2025-02-16 - Added detailed configuration documentation and converted to proper C++ structure
*Updated 2025-02-26 - Added support for disabling specific servos (set pin to -1) while maintaining gate numbering
*Updated 2025-02-26 - Fixed button timing logic to properly open gates after selection
*Updated 2025-02-26 - Improved timer logic to ensure gates open/close reliably, reduced OPEN_DELAY to 800ms
*Updated 2025-02-26 - Implemented proper button debouncing and state machine for more reliable operation
*Updated 2025-02-26 - Fixed issue with servos not moving when button was pressed
*Updated 2025-02-26 - Enhanced gate selection to automatically skip disabled gates when cycling through options
*Updated 2025-02-27 - Fixed compiler warnings in Configuration.h (removed redundant header guard and corrected version number format) and in GateServos.h (fixed signed/unsigned comparison by changing opendelay type to unsigned long)
*Updated 2025-02-27 - Added gate orientation configuration (GATE_CLOSED_AT_MAX_x) to support different physical gate setups where gates may be open when up or closed when up
