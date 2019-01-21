
#ifndef CONFIGURATION_H
#define CONFIGURATION_H 
#define CONFIGURATION_H_VERSION 01

// All user defined settings required to configure operation are in this file

//#define DEBUG   //Enable debug output
//#define DEBUGMETER // debug meter mode for placing sensors on cables


// servo stuff 
#define CLOSE_DELAY 1000;// how long it takes a gate to close
#define OPEN_DELAY 1000; // how long after last button push to open gate

#define HAS_BUTTON true;  // true if button is attached
#define BUTTON_PIN 13;    // the number of the pushbutton pin

/// current Sensor stuff
#define NUM_AC_SENSORS 4;        // Number of AC sensors connected.. currently only handles 1 per gate
#define NUM_LEDS 4;              // Number of LEDs connected.. optional but should be 1 per gate
#define NUM_GATES 4;             // Number of blast gates with servos connected
#define NUM_OFF_SAMPLES 50;      // number of samples when checking avg sensor off values (unused)
#define NUM_OFF_MAX_SAMPLES 500; // Milliseconds to sample for max off value for each gate when starting up
#define AVG_READINGS 25;         // number of readings to average when triggering gates.. higher number is more accurate but more delay ( no more than 50)
#define AC_SENSOR_SENSITIVITY 2.0; // Triggers on twice the max readings of the off setting. The closer to one, the more sensitive


#ifdef DEBUG
#define MAX_BLINK_LEN 10; // blink faster when debugging to serial since slower
#else 
#define MAX_BLINK_LEN 1000 / NUM_AC_SENSORS; // slowest blink rate in ms
#endif

// Servo Pins
#define SERVO_PIN_1 12; // Pin for first blast gate servo
#define SERVO_PIN_2 10; // ...etc..
#define SERVO_PIN_3 8;
#define SERVO_PIN_4 9;
#define SERVO_PIN_5 -1;
#define SERVO_PIN_6 -1;
#define SERVO_PIN_7 -1; 
#define SERVO_PIN_8 -1;

// AC Sensor Pins
#define AC_SENSOR_PIN_1 A0; // Pin for first AC sensor
#define AC_SENSOR_PIN_2 A1; // ..etc...
#define AC_SENSOR_PIN_3 A2;
#define AC_SENSOR_PIN_4 A3;
#define AC_SENSOR_PIN_5 -1;
#define AC_SENSOR_PIN_6 -1;
#define AC_SENSOR_PIN_7 -1;
#define AC_SENSOR_PIN_8 -1;


// Servo Max (Closed) positions.. maximum close position representing closed.
// It's ok to leave at default and for the servo to try to close too far but it will increase the power load
//
#define SERVO_MAX_1 180;
#define SERVO_MAX_2 180;
#define SERVO_MAX_3 180;
#define SERVO_MAX_4 180;
#define SERVO_MAX_5 -1;
#define SERVO_MAX_6 -1;
#define SERVO_MAX_7 -1;
#define SERVO_MAX_8 -1;

// Servo Min (Open) positions
// It's ok to leave at default and for the servo to try to open too far but it will increase the power load
//
#define SERVO_MIN_1 0;
#define SERVO_MIN_2 0;
#define SERVO_MIN_3 0;
#define SERVO_MIN_4 0;
#define SERVO_MIN_5 -1;
#define SERVO_MIN_6 -1;
#define SERVO_MIN_7 -1;
#define SERVO_MIN_8 -1;

// LED pins
#define LED_PIN_1 3;  // Pin for first LED indicator
#define LED_PIN_2 4;  // ...etc..
#define LED_PIN_3 5;
#define LED_PIN_4 6;
#define LED_PIN_5 -1;
#define LED_PIN_6 -1;
#define LED_PIN_7 -1;
#define LED_PIN_8 -1;


#endif // CONFIGURATION_H
