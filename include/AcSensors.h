/*
  AcSensors.h - Library for http://moderndevice.com/current AC sensors
  Created by Greg Pringle 2019-01-20
  Released into the public domain.
*/
#ifndef AcSensor_h
#define AcSensor_h

#include "Arduino.h"
#include <Servo.h>
#include "Debug.h"
#include "Configuration.h"

  class AcSensors {
     
    static const int ac_sensor_1 = AC_SENSOR_PIN_1;
    static const int ac_sensor_2 = AC_SENSOR_PIN_2;
    static const int ac_sensor_3 = AC_SENSOR_PIN_3; 
    static const int ac_sensor_4 = AC_SENSOR_PIN_4;
    static const int ac_sensor_5 = AC_SENSOR_PIN_5;
    static const int ac_sensor_6 = AC_SENSOR_PIN_6;
    static const int ac_sensor_7 = AC_SENSOR_PIN_7;
    static const int ac_sensor_8 = AC_SENSOR_PIN_8;

    static const int led_pin_1 = LED_PIN_1;
    static const int led_pin_2 = LED_PIN_2;
    static const int led_pin_3 = LED_PIN_3;
    static const int led_pin_4 = LED_PIN_4;
    static const int led_pin_5 = LED_PIN_5;
    static const int led_pin_6 = LED_PIN_6;
    static const int led_pin_7 = LED_PIN_7;
    static const int led_pin_8 = LED_PIN_8;
    
    static const float acsensorsentitivity;
    static const int numoffmaxsamples = NUM_OFF_MAX_SAMPLES;
    static const int maxblinklen = MAX_BLINK_LEN;
    static const int numoffsamples = NUM_OFF_SAMPLES;
    static const int avg_readings = AVG_READINGS;
    static const int ac_sensors = NUM_AC_SENSORS;
    static const int max_sensors = 8;

    // Flutter protection constants
    static const float sensitivityOn = AC_SENSOR_SENSITIVITY_ON;
    static const float sensitivityOff = AC_SENSOR_SENSITIVITY_OFF;
    static const int debounceStableReadings = DEBOUNCE_STABLE_READINGS;

    int curreadingindex = 0;
    int blinktimers[max_sensors] = {0,0,0,0,0,0,0,0};
    bool blinkon[max_sensors]= {false, false, false, false, false, false, false, false};
    const int sensorPins[max_sensors] = { ac_sensor_1, ac_sensor_2, ac_sensor_3, ac_sensor_4, ac_sensor_5, ac_sensor_6, ac_sensor_7, ac_sensor_8 };
    const int ledpin[max_sensors] = {led_pin_1,led_pin_2,led_pin_3,led_pin_4,led_pin_5,led_pin_6,led_pin_7,led_pin_8}; // LED pins
    float offReadings[ac_sensors];
    int recentReadings[ac_sensors][avg_readings];
    
    // Flutter protection state tracking
    bool sensorState[max_sensors] = {false, false, false, false, false, false, false, false}; // Current state (true = tool on)
    int debounceCounter[max_sensors] = {0, 0, 0, 0, 0, 0, 0, 0}; // Consecutive readings in desired state
    
    public:    
      AcSensors();
      void InitializeSensors();               // Initialize sensors and read a baseline sensor reading with tools off
      float AvgSensorReading(int forsensor);  // returns an average of the last X sensors readings for given sensor
      bool Triggered(int sensor);             // Returns true if the given AC current sensor number is triggered     
      void getMaxOffSensorReadings();         // Poll for NUM_OFF_MAX_SAMPLES ms to determine maximum 'off' sensor reading for this sensor
      void getAvgOffSensorReadings();         // Determine average 'off' reading for each sensor. 
      void ReadSensors();                     // Read values for AC current sensors and add to list of values we will average
      void DisplayMeter();                    // Use LEDs to display a meter for positioning AC sensor clamps. 
      void displayaverages(int cursensor);    // Debugging function to display values polled for given sensor
      float GetOffReading(int sensor);        // Get the off reading for a specific sensor
      float GetAvgReading(int sensor);        // Get the average reading for a specific sensor
      static const int num_ac_sensors = NUM_AC_SENSORS;
  };
  

#endif
