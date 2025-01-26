# BlastGateServo
Blast gate servo controller for Arduino

See 3d printed parts at : https://www.thingiverse.com/thing:3301904

## Features
* Allows a single push button to switch between different blast gates, with only a single one open at a time
* Currently open blast gate is indicated by an LED
* Optional automatic control using AC current sensors to detect active tools

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
4. PlatformIO will automatically install required dependencies

## Building and Uploading
1. Connect your Arduino Uno to your computer via USB
2. In VSCode:
   - Click the PlatformIO icon in the sidebar
   - Under Project Tasks:
     - Click "Build" to compile
     - Click "Upload" to flash to Arduino
   - Use "Monitor" to view serial output (when debugging is enabled)

## Operation Modes
* Normal Mode: Use the push button to cycle through gates. After selecting a gate, wait briefly and it will open automatically.
* Meter Mode: For calibrating AC sensors. Enter this mode by holding the button while powering up the Arduino.

## Configuration
All settings can be adjusted in Configuration.h:
* Number of gates (NUM_GATES)
* Pin assignments for servos, LEDs, and sensors
* Timing delays and sensitivity settings

## Project Structure
* BlastGateServo.ino - Main program file
* Configuration.h - All user configurable settings
* GateServos.h/cpp - Servo control implementation
* AcSensors.h/cpp - AC current sensor implementation
* Debug.h - Debug output macros

Created 2019-01-02 - Greg Pringle
Updated 2019-01-20 - Greg Pringle - Added AC sensor support
Updated 2025-01-26 - Added VSCode/PlatformIO support
