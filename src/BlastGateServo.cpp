#include <Arduino.h>
#include <Servo.h>
#include "Debug.h"
#include "Configuration.h"
#include "GateServos.h"
#include "AcSensors.h"

/*  Blast gate servo controller for Arduino
 *   
 *   Allows a single push button to switch between different blast gates, with only a single one open at a time.
 *   Currently open blast gate is indicated by an LED.
 *    
 *   Requires:  A servo for each blast gate, one button
 *   Optional: An LED for each blast gate
 *   
 *   In the future I plan on adding an option to automate this based on detected AC current from the tool
 *   (if my sensor ever shows up)
 *   
 *   Created 2019-01-02 - Greg Pringle
 *   Updated 2019-01-20 - Greg Pringle - Refactored, added support for AC current sensor
 */

int buttonState = 0;         // variable for reading the pushbutton status
int curselectedgate = -1;    // Currently selected gate (-1 = all closed)
bool metermode = false;      // Meter mode flag
bool toolon = false;         // indicates if there is any current sensed

// Button handling variables
int lastButtonState = HIGH;  // Previous button state (HIGH = not pressed with pull-up)
unsigned long lastDebounceTime = 0;  // Last time button state changed
unsigned long debounceDelay = 50;    // Debounce time in milliseconds
unsigned long gateOpenTimer = 0;     // Timer for gate opening delay
bool gateSelectionActive = false;    // Flag to indicate a gate has been selected and waiting to open

static const bool has_button = HAS_BUTTON;
static const int buttonPin = BUTTON_PIN;
static const bool hasbutton = HAS_BUTTON;  

GateServos gateservos(-1);  // object controlling blast gate servos
AcSensors acsensors;        // object controlling AC current sensors

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  delay(1000);  // Give serial connection time to establish
  DPRINTLN("BlastGateServo starting...");
  #endif

  // Set up button pin for all modes
  if (has_button) {
      pinMode(buttonPin, INPUT_PULLUP);
      delay(100); // Give pin time to stabilize
      
      // Initialize button state to prevent false detection at startup
      buttonState = digitalRead(buttonPin);
      lastButtonState = buttonState;
      lastDebounceTime = millis();
      gateSelectionActive = false;
      gateOpenTimer = 0;
      
      DPRINTLN("Button initialized");
  }

  #ifdef DEBUG_SERVO_TEST
  // For servo test mode, we only need to set up the button
  // and initialize the servo pin for the selected servo
  DPRINT("Servo Test Mode Active - Press button to toggle servo ");
  DPRINTLN(TEST_SERVO_INDEX);
  
  // Initialize just the LED pin for the selected servo
  int ledPin;
  // Adjust for 1-based indexing in display but 0-based indexing in arrays
  switch(TEST_SERVO_INDEX - 1) {
    case 0: ledPin = LED_PIN_1; break;
    case 1: ledPin = LED_PIN_2; break;
    case 2: ledPin = LED_PIN_3; break;
    case 3: ledPin = LED_PIN_4; break;
    case 4: ledPin = LED_PIN_5; break;
    default: ledPin = LED_PIN_1; break;
  }
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Skip all other initialization
  return;
  #endif

  // Check for meter mode
  if (has_button && digitalRead(buttonPin) == LOW) {
      metermode = true;
      DPRINTLN("Entering meter mode - Use LEDs to calibrate AC sensor positions");
  } else {
      metermode = false;
  }

  #ifdef DEBUGMETER
      metermode = true;
  #endif

  // Initialize sensors before anything else
  acsensors.InitializeSensors();

  // Initialize gates if not in meter mode
  if (!metermode) {
      gateservos.initializeGates();
  }

  #if !ENABLE_AC_SENSORS
  // Print this message only once during setup
  DPRINTLN("AC sensors disabled - using manual control only");
  #endif

  #ifdef DEBUG_LED_TEST
  // Initialize LED pins for test mode
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
  pinMode(LED_PIN_5, OUTPUT);
  
  // Start with all LEDs off
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);
  digitalWrite(LED_PIN_4, LOW);
  digitalWrite(LED_PIN_5, LOW);
  
  DPRINTLN("LED Test Mode Active - Press button to light all LEDs");
  #endif
  // Note: Removed duplicate initialization
}


void loop()
{
  #ifdef DEBUG_LED_TEST
  // LED test pattern - flash each LED in sequence or all on when button pressed
  static int currentLed = 0;
  static unsigned long lastChange = 0;
  const int testDelay = 500; // 500ms on/off time

  // Check button state (LOW when pressed due to pull-up resistor)
  int btnState = digitalRead(BUTTON_PIN);
  DPRINT("Button state: ");
  DPRINTLN(btnState);
  
  if (btnState == LOW) {
    // Button pressed (reads LOW with pull-up) - turn on all LEDs
    digitalWrite(LED_PIN_1, HIGH);
    digitalWrite(LED_PIN_2, HIGH);
    digitalWrite(LED_PIN_3, HIGH);
    digitalWrite(LED_PIN_4, HIGH);
    digitalWrite(LED_PIN_5, HIGH);
  } else {
    // Button not pressed - do sequential pattern
    if (millis() - lastChange >= testDelay) {
      // Turn off all LEDs first
      digitalWrite(LED_PIN_1, LOW);
      digitalWrite(LED_PIN_2, LOW);
      digitalWrite(LED_PIN_3, LOW);
      digitalWrite(LED_PIN_4, LOW);
      digitalWrite(LED_PIN_5, LOW);

      // Turn on only the current LED
      switch(currentLed) {
        case 0: digitalWrite(LED_PIN_1, HIGH); break;
        case 1: digitalWrite(LED_PIN_2, HIGH); break;
        case 2: digitalWrite(LED_PIN_3, HIGH); break;
        case 3: digitalWrite(LED_PIN_4, HIGH); break;
        case 4: digitalWrite(LED_PIN_5, HIGH); break;
      }

      // Move to next LED
      currentLed = (currentLed + 1) % 5; // Use explicit 5 instead of NUM_LEDS
      lastChange = millis();
    }
  }
  return; // Skip normal operation when in LED test mode
  #endif

  #ifdef DEBUG_SERVO_TEST
  // Servo test mode - open/close selected servo with button press
  static bool servoOpen = false;
  static bool lastButtonState = HIGH;
  static Servo testServo;  // Local servo object for test mode
  
  // Get the pin configurations for the selected servo
  static int servoPin, servoMax, servoMin, ledPin;
  static bool configPrinted = false;
  
  // Only set up the configuration once
  if (!configPrinted) {
    // Adjust for 1-based indexing in display but 0-based indexing in arrays
    switch(TEST_SERVO_INDEX - 1) {
      case 0:
        servoPin = SERVO_PIN_1;
        servoMax = SERVO_MAX_1;
        servoMin = SERVO_MIN_1;
        ledPin = LED_PIN_1;
        break;
      case 1:
        servoPin = SERVO_PIN_2;
        servoMax = SERVO_MAX_2;
        servoMin = SERVO_MIN_2;
        ledPin = LED_PIN_2;
        break;
      case 2:
        servoPin = SERVO_PIN_3;
        servoMax = SERVO_MAX_3;
        servoMin = SERVO_MIN_3;
        ledPin = LED_PIN_3;
        break;
      case 3:
        servoPin = SERVO_PIN_4;
        servoMax = SERVO_MAX_4;
        servoMin = SERVO_MIN_4;
        ledPin = LED_PIN_4;
        break;
      case 4:
        servoPin = SERVO_PIN_5;
        servoMax = SERVO_MAX_5;
        servoMin = SERVO_MIN_5;
        ledPin = LED_PIN_5;
        break;
      default:
        servoPin = SERVO_PIN_1;
        servoMax = SERVO_MAX_1;
        servoMin = SERVO_MIN_1;
        ledPin = LED_PIN_1;
        break;
    }
    
    // Print debug info about the selected servo (only once)
    DPRINT("Servo ");
    DPRINT(TEST_SERVO_INDEX);
    DPRINT(" configuration - Pin: ");
    DPRINT(servoPin);
    DPRINT(", Max (closed): ");
    DPRINT(servoMax);
    DPRINT(", Min (open): ");
    DPRINTLN(servoMin);
    
    // Add a warning if the servo pin is disabled
    if (servoPin == -1) {
      DPRINTLN("WARNING: Selected servo pin is disabled (-1)");
      DPRINTLN("LED will still work but servo will not move");
    }
    
    configPrinted = true;
  }
  
  // Check button state (LOW when pressed due to pull-up resistor)
  int btnState = digitalRead(BUTTON_PIN);
  
  // Button state changed from HIGH to LOW (button pressed)
  if (btnState == LOW && lastButtonState == HIGH) {
    DPRINTLN("Button pressed in servo test mode");
    
    // Toggle servo state and LED
    if (servoOpen) {
      // Close the servo
      DPRINT("Closing servo ");
      DPRINT(TEST_SERVO_INDEX);
      DPRINT(" - Setting position to ");
      DPRINTLN(servoMax);
      digitalWrite(ledPin, LOW);
      
      // Only control the servo if the pin is valid (not -1)
      if (servoPin != -1) {
        // Direct servo control without using gateservos
        testServo.attach(servoPin);
        testServo.write(servoMax);
        delay(CLOSE_DELAY);
        testServo.detach();
        DPRINTLN("Servo closed");
      } else {
        DPRINTLN("Skipped servo (pin disabled)");
        delay(CLOSE_DELAY); // still delay for consistency
      }
      
      servoOpen = false;
    } else {
      // Open the servo
      DPRINT("Opening servo ");
      DPRINT(TEST_SERVO_INDEX);
      DPRINT(" - Setting position to ");
      DPRINTLN(servoMin);
      digitalWrite(ledPin, HIGH);
      
      // Only control the servo if the pin is valid (not -1)
      if (servoPin != -1) {
        // Direct servo control without using gateservos
        testServo.attach(servoPin);
        testServo.write(servoMin);
        delay(OPEN_DELAY);
        testServo.detach();
        DPRINTLN("Servo opened");
      } else {
        DPRINTLN("Skipped servo (pin disabled)");
        delay(OPEN_DELAY); // still delay for consistency
      }
      
      servoOpen = true;
    }
    
    delay(200); // Debounce delay
  }
  
  lastButtonState = btnState;
  return; // Skip normal operation when in servo test mode
  #endif

  bool toolon = false; // indicates if there is any current sensed

  #if ENABLE_AC_SENSORS
  // Only process AC sensors if they are enabled
  acsensors.ReadSensors(); // read all the AC current sensors
  
  if (metermode) {
      acsensors.DisplayMeter();  // if user put device into meter mode, use LED lights to display sensor signal.
      return; // Don't process any other logic in meter mode
  }
  else // not meter mode
  {
    for (int cursensor=0; cursensor < acsensors.num_ac_sensors; cursensor++)
    {
        // This sensor is triggered by power tool
        //
        if (acsensors.Triggered(cursensor))
        {
          // this tool is active, output info to debug
          DPRINT(" TOOL ON #"); DPRINT(cursensor); DPRINT(" OFF READING:"); DPRINT(acsensors.GetOffReading(cursensor)); DPRINT(" AVG SENSOR READING:"); DPRINTLN(acsensors.GetAvgReading(cursensor));

          // ignore button if tool detected
          gateSelectionActive = false;
          toolon = true;
          if (curselectedgate != cursensor) curselectedgate = cursensor;

          // Gate hasn't been opened yet, open it
          if (gateservos.gateopen[cursensor] != true)
          {
            gateservos.gateopen[cursensor] = true;
            gateservos.ledon(cursensor);
            gateservos.opengate(cursensor);
          }
        }
        else
        {
          // this tool is not active and gate hasn't been closed yet. Close it.
          if (gateservos.gateopen[cursensor])
          {
            gateservos.gateopen[cursensor] = false;
            gateservos.ledoff(cursensor);
            DPRINT(" TOOL OFF #"); DPRINTLN(cursensor);
            gateservos.closegate(cursensor);
            curselectedgate = gateservos.firstgateopen();  // change currently active gate to first open one
          }
        }
    }
  }
  #else
  // AC sensors are disabled, only manual control is available
  // Debug message moved to setup
  #endif

    // Button handling with debounce and state machine
    if (hasbutton && !toolon) {
      // Read the current button state
      int reading = digitalRead(buttonPin);
      
      // Check if button state has changed
      if (reading != lastButtonState) {
        // Reset debounce timer
        lastDebounceTime = millis();
      }
      
      // If button state has been stable for debounce period
      if ((millis() - lastDebounceTime) > debounceDelay) {
        // If button state has changed since last stable reading
        if (reading != buttonState) {
          buttonState = reading;
          
          // Button pressed (LOW with pull-up resistor)
          if (buttonState == LOW) {
            // Show the previous selection before changing it
            if (curselectedgate == -1) {
              DPRINTLN("Button Pressed - Previous selection: All gates closed");
            } else {
              DPRINT("Button Pressed - Previous selection: Gate #");
              DPRINTLN(curselectedgate + 1);
            }
            
            // Turn off LED for previous selection
            gateservos.ledoff(curselectedgate);
            
            // Move to next gate
            curselectedgate++;
            if (curselectedgate == gateservos.num_gates)
              curselectedgate = -1;
            else
              gateservos.ledon(curselectedgate);
            
            // Display new selection that will be opened after delay
            if (curselectedgate == -1) {
              DPRINTLN("New selection: All gates closed (will close all gates after delay)");
            } else {
              DPRINT("New selection: Gate #");
              DPRINTLN(curselectedgate + 1);
              DPRINTLN("(will open after delay)");
            }
          }
          // Button released (HIGH with pull-up resistor)
          else if (buttonState == HIGH) {
            DPRINTLN("Button released, starting gate open timer");
            gateSelectionActive = true;
            gateOpenTimer = millis();
          }
        }
      }
      
      // Update last button state
      lastButtonState = reading;
      
      // If a gate is selected and we're waiting for the delay to open it
      if (gateSelectionActive) {
        // Calculate elapsed time
        unsigned long elapsedTime = millis() - gateOpenTimer;
        
        // Debug output every 200ms to avoid flooding serial
        if (elapsedTime % 200 < 50) {
          DPRINT("Timer: ");
          DPRINTLN(elapsedTime);
          
          DPRINT("Current selected gate: ");
          if (curselectedgate == -1) {
            DPRINTLN("All gates closed");
          } else {
            DPRINTLN(curselectedgate + 1);
          }
          
          DPRINT("Current open gate: ");
          if (gateservos.curopengate == -1) {
            DPRINTLN("None");
          } else {
            DPRINTLN(gateservos.curopengate + 1);
          }
          
          DPRINT("Threshold: ");
          DPRINTLN(gateservos.opendelay);
        }
        
        // Open the gate when we reach the threshold
        if (elapsedTime >= gateservos.opendelay) {
          DPRINTLN("Opening gate after delay");
          
          // Debug the gate we're about to open
          if (curselectedgate == -1) {
            DPRINTLN("Opening: All gates closed");
          } else {
            DPRINT("Opening: Gate #");
            DPRINTLN(curselectedgate + 1);
          }
          
          // Call the function to open/close gates
          gateservos.ManuallyOpenGate(curselectedgate);
          
          // Reset gate selection
          gateSelectionActive = false;
        }
      }
    }
  
  if (metermode)
   delay(1);  // minimal delay while metering so we can collect as many samples as possible
  else
   delay(50);
  
}