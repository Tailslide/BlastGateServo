#ifndef CONFIGURATION_H
#define CONFIGURATION_H_VERSION 1

// All user defined settings required to configure operation are in this file

//#ifndef DEBUG
//#define DEBUG   //Enable debug output - set this in platformio.ini instead
//#endif
//#define DEBUGMETER // debug meter mode for placing sensors on cables
//#define DEBUG_METER_VERBOSE // Enable verbose debug output in meter mode (slows down LED blinking)
//#define DEBUG_LED_TEST // Enable LED test mode - flashes LEDs in sequence
//#define DEBUG_SERVO_TEST // Enable servo test mode - opens/closes a servo with button press
#define TEST_SERVO_INDEX 1 // Which servo to test (1 = first servo, 2 = second servo, etc.)
//#define DEBUG_SENSOR_TEST // Enable sensor test mode - display raw readings for a specific sensor
//#define TEST_SENSOR_INDEX 1 // Which sensor to test (1 = first sensor, 2 = second sensor, etc.)

// Set to false to disable AC sensor functionality (manual button control only)
#define ENABLE_AC_SENSORS true


// servo stuff 
#define CLOSE_DELAY 1000 // how long it takes a gate to close
#define OPEN_DELAY 800 // how long after last button push to open gate

#define HAS_BUTTON true  // true if button is attached
#define BUTTON_PIN 13    // the number of the pushbutton pin

/// current Sensor stuff
#define NUM_AC_SENSORS 5        // Number of AC sensors connected.. currently only handles 1 per gate
#define NUM_LEDS 5              // Number of LEDs connected.. optional but should be 1 per gate
#define NUM_GATES 5             // Number of blast gates with servos connected
#define NUM_OFF_SAMPLES 50      // number of samples when checking avg sensor off values (unused)
#define NUM_OFF_MAX_SAMPLES 500 // Milliseconds to sample for max off value for each gate when starting up
#define AVG_READINGS 25         // number of readings to average when triggering gates.. higher number is more accurate but more delay ( no more than 50)
#define AC_SENSOR_SENSITIVITY 2.0 // Triggers on twice the max readings of the off setting. The closer to one, the more sensitive

// Flutter Protection Settings
#define AC_SENSOR_SENSITIVITY_ON  2.0  // Threshold to turn tool ON (same as AC_SENSOR_SENSITIVITY for backward compatibility)
#define AC_SENSOR_SENSITIVITY_OFF 1.5  // Threshold to turn tool OFF (hysteresis prevents rapid toggling)
#define DEBOUNCE_STABLE_READINGS  3    // Number of consecutive stable readings required before state change
#define MIN_SERVO_INTERVAL_MS     2000 // Minimum milliseconds between servo operations on same gate
#define MAX_OPS_PER_MINUTE        10   // Maximum operations per minute before emergency shutdown
#define ERROR_FLASH_INTERVAL_MS   200  // LED flash interval in error state (milliseconds)


// Blink timing for meter mode (in milliseconds)
#ifdef DEBUG
#define MAX_BLINK_LEN 50  // faster blink in debug mode to compensate for serial delay
#else
#define MAX_BLINK_LEN 200 // normal blink rate in release mode
#endif

// Servo Pins
#define SERVO_PIN_1 12 // Pin for first blast gate servo
#define SERVO_PIN_2 -1 // ...etc..
#define SERVO_PIN_3 11
#define SERVO_PIN_4 9
#define SERVO_PIN_5 8
#define SERVO_PIN_6 -1
#define SERVO_PIN_7 -1
#define SERVO_PIN_8 -1

// AC Sensor Pins
#define AC_SENSOR_PIN_1 A0 // Pin for first AC sensor
#define AC_SENSOR_PIN_2 -1 //A1 // ..etc...
#define AC_SENSOR_PIN_3 A2
#define AC_SENSOR_PIN_4 A3
#define AC_SENSOR_PIN_5 A4 //A4
#define AC_SENSOR_PIN_6 -1
#define AC_SENSOR_PIN_7 -1
#define AC_SENSOR_PIN_8 -1

// Gate orientation configuration
// Set to true if gate is closed when servo is at max position (default)
// Set to false if gate is open when servo is at max position (inverted)
#define GATE_CLOSED_AT_MAX_1 false
#define GATE_CLOSED_AT_MAX_2 false
#define GATE_CLOSED_AT_MAX_3 false
#define GATE_CLOSED_AT_MAX_4 false
#define GATE_CLOSED_AT_MAX_5 false
#define GATE_CLOSED_AT_MAX_6 false
#define GATE_CLOSED_AT_MAX_7 false
#define GATE_CLOSED_AT_MAX_8 false

// Servo Max positions (180 degrees)
// It's ok to leave at default and for the servo to try to go too far but it will increase the power load
//
#define SERVO_MAX_1 180
#define SERVO_MAX_2 180
#define SERVO_MAX_3 180
#define SERVO_MAX_4 180
#define SERVO_MAX_5 180
#define SERVO_MAX_6 -1
#define SERVO_MAX_7 -1
#define SERVO_MAX_8 -1

// Servo Min positions (0 degrees)
// It's ok to leave at default and for the servo to try to go too far but it will increase the power load
//
#define SERVO_MIN_1 0
#define SERVO_MIN_2 0
#define SERVO_MIN_3 0
#define SERVO_MIN_4 0
#define SERVO_MIN_5 0
#define SERVO_MIN_6 -1
#define SERVO_MIN_7 -1
#define SERVO_MIN_8 -1



// LED pins
#define LED_PIN_1 6  // Pin for first LED indicator
#define LED_PIN_2 -1 //2  // ...etc..
#define LED_PIN_3 3
#define LED_PIN_4 4
#define LED_PIN_5 5
#define LED_PIN_6 -1
#define LED_PIN_7 -1
#define LED_PIN_8 -1


#endif // CONFIGURATION_H
