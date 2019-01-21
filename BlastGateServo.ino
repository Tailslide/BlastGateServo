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
#include <Servo.h>
#include "Debug.h"
#include "Configuration.h"
#include "GateServos.h"
#include "AcSensors.h"

int buttonState = 0;         // variable for reading the pushbutton status
int lastbuttonpushms = 0;
int curselectedgate = -1;
bool waitingforbuttonrelease = false;
bool metermode = false; 

static const bool has_button = HAS_BUTTON;
static const int buttonPin = BUTTON_PIN;
static const bool hasbutton = HAS_BUTTON;  

GateServos gateservos(-1);  // object controlling blast gate servos
AcSensors acsensors;        // objest controlling AC current sensors

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  gateservos.initializeGates();

  // initialize the pushbutton pin as an input:
  if (has_button) pinMode(buttonPin, INPUT);
  
  if (digitalRead(buttonPin)== HIGH) // user held down button on startup, go into meter mode
      metermode = true;
  else metermode = false;

  #ifdef DEBUGMETER
      metermode = true;
  #endif
  acsensors.InitializeSensors();
}


void loop() 
{
  bool toolon = false; // indicates if there is any current sensed

  acsensors.ReadSensors(); // read all the AC current sensors
  
  if (metermode)  acsensors.DisplayMeter();  // if user put device into meter mode, use LED lights to display sensor signal.
  else // not meter mode
  {
    for (int cursensor=0; cursensor < acsensors.num_ac_sensors; cursensor++)
    {
        // This sensor is triggered by power tool
        //
        if (acsensors.Triggered(cursensor))
        {
          // this tool is active, output info to debug
          DPRINT(" TOOL ON #"); DPRINT(cursensor); DPRINT(" OFF READING:"); DPRINT(offReadings[cursensor]); DPRINT(" AVG SENSOR READING:"); DPRINTLN(avgSensorReading(cursensor));

          // ignore button if tool detected
          waitingforbuttonrelease = false;
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

    // read the state of the pushbutton value.  
    // Allow the user to light up the appropriate gate and pause before opening/closing the gates.
    if (hasbutton) buttonState = digitalRead(buttonPin);
    if (hasbutton && !toolon && buttonState == HIGH)
    {
      if (! waitingforbuttonrelease)
      {
        lastbuttonpushms = 0;
        DPRINT("Button Pushed, curselectedgate= ");  DPRINTLN(curselectedgate);
        gateservos.ledoff(curselectedgate);
        curselectedgate++;
        if (curselectedgate == gateservos.num_gates) 
          curselectedgate = -1;
        else
          gateservos.ledon(curselectedgate);
        waitingforbuttonrelease= true;
      }
    }
    
    if (hasbutton && !toolon && buttonState == LOW)
    {
      if (waitingforbuttonrelease)
      {
         waitingforbuttonrelease=false;
         lastbuttonpushms = 0;
      }
    
      if (lastbuttonpushms > gateservos.opendelay)
      {    
          gateservos.ManuallyOpenGate(curselectedgate);
      }
    }
  }
  
  if (metermode)
   delay(1);  // minimal delay while metering so we can collect as many samples as possible
  else 
   delay(50);
  if (!toolon)
  {    
     // User is picking which gate to open manually. this code 'debounces' the button so it only moves one at a time
     //
     if (curselectedgate == gateservos.curopengate) 
        lastbuttonpushms = 0;
     else
        lastbuttonpushms +=50;
  }
  
}
