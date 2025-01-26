/*
  GateServos.h - Library for operating blast gate servos
  Created by Greg Pringle 2019-01-20
  Released into the public domain.
*/
#ifndef GateServo_h
#define GateServo_h

#include "Arduino.h"
#include <Servo.h>
#include "Debug.h"
#include "Configuration.h"

  class GateServos {
    static const int num_servos = NUM_GATES;
    static const int num_leds = NUM_LEDS;
    static const int servo_pin_1 = SERVO_PIN_1;
    static const int servo_pin_2 = SERVO_PIN_2; 
    static const int servo_pin_3 = SERVO_PIN_3;
    static const int servo_pin_4 = SERVO_PIN_4;
    static const int servo_pin_5 = SERVO_PIN_5;
    static const int servo_pin_6 = SERVO_PIN_6;
    static const int servo_pin_7 = SERVO_PIN_7;
    static const int servo_pin_8 = SERVO_PIN_8;

    static const int servo_max_1 = SERVO_MAX_1;
    static const int servo_max_2 = SERVO_MAX_2;
    static const int servo_max_3 = SERVO_MAX_3;
    static const int servo_max_4 = SERVO_MAX_4;
    static const int servo_max_5 = SERVO_MAX_5;
    static const int servo_max_6 = SERVO_MAX_6;
    static const int servo_max_7 = SERVO_MAX_7;
    static const int servo_max_8 = SERVO_MAX_8;

    static const int servo_min_1 = SERVO_MIN_1;
    static const int servo_min_2 = SERVO_MIN_2;
    static const int servo_min_3 = SERVO_MIN_3;
    static const int servo_min_4 = SERVO_MIN_4;
    static const int servo_min_5 = SERVO_MIN_5;
    static const int servo_min_6 = SERVO_MIN_6;
    static const int servo_min_7 = SERVO_MIN_7;
    static const int servo_min_8 = SERVO_MIN_8;

    static const int led_pin_1 = LED_PIN_1;
    static const int led_pin_2 = LED_PIN_2;
    static const int led_pin_3 = LED_PIN_3;
    static const int led_pin_4 = LED_PIN_4;
    static const int led_pin_5 = LED_PIN_5;
    static const int led_pin_6 = LED_PIN_6;
    static const int led_pin_7 = LED_PIN_7;
    static const int led_pin_8 = LED_PIN_8;
    static const int closedelay = CLOSE_DELAY;

    const int servopin[8] = {servo_pin_1,servo_pin_2,servo_pin_3,servo_pin_4,servo_pin_5,servo_pin_6,servo_pin_7,servo_pin_8}; 
    const int maxservo[8] = {servo_max_1,servo_max_2,servo_max_3,servo_max_4,servo_max_5,servo_max_6,servo_max_7,servo_max_8};
    const int minservo[8] = {servo_min_1,servo_min_2,servo_min_3,servo_min_4,servo_min_5,servo_min_6,servo_min_7,servo_min_8};
    const int ledpin[8] = {led_pin_1,led_pin_2,led_pin_3,led_pin_4,led_pin_5,led_pin_6,led_pin_7,led_pin_8}; // LED pins    

    
    Servo myservo;  // create servo object to control a servo
             // a maximum of eight servo objects can be created
    
    public:    
      GateServos(int curopengate);  // initialize indicating currenly open gate (usually -1 for none)
      void opengate(int gatenum);   // open the given gate number
      void closegate(int gatenum);  // close the given gate number
      void initializeGates();       // initialize gates and close them all
      void ledoff(int gatenum);     // turn off given LED
      void ledon(int gatenum);      // turn on given LED
      void ManuallyOpenGate(int gatenum);   // User manually opening given gate using the button
      int firstgateopen();                  // Returns ID of first gate that is open
      void testServo(int servopin);         // Test given servo pin (debug function)
      const int num_gates = NUM_GATES;      // 
      int curopengate = -1;                 // cuurrently open gate selected manually with button
      const int opendelay = OPEN_DELAY;     // ms delay to allow servo to completely open gate
      bool gateopen[8] = {false, false, false, false,false, false, false, false};   // array indicating which gates are open
  };
  

#endif
