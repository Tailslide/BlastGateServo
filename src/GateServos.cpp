#include "Arduino.h"
#include <Servo.h>
#include "Debug.h"
#include "Configuration.h"
#include "GateServos.h"

  // Constructor.. usually called with -1 to indicate no gates are open
  //
  GateServos::GateServos(int initcuropengate)
  { 
    curopengate = initcuropengate;
  }

  // Open the given gate number
  //
  void GateServos::opengate(int gatenum)
  {
      DPRINT("OPENING GATE #");
      DPRINT(gatenum);
      DPRINT(" SERVO PIN:");
      DPRINT(servopin[gatenum]);
      DPRINT(" VALUE:");
      DPRINT(minservo[gatenum]);
      DPRINT(" DELAY:");
      DPRINT(opendelay);
      DPRINTLN("");
      
      curopengate = gatenum;
      digitalWrite(ledpin[gatenum], HIGH);
      
      // Only control the servo if the pin is valid (not -1)
      if (servopin[gatenum] != -1) {
        myservo.attach(servopin[gatenum]);  // attaches the servo
        myservo.write(minservo[gatenum]); //open gate
        delay(opendelay); // wait for gate to open
        myservo.detach();
        DPRINTLN("OPENED GATE");
      } else {
        DPRINTLN("SKIPPED SERVO (PIN DISABLED)");
        delay(opendelay); // still delay for consistency
      }
      //if (servopin[gatenum] ==12) testServo(12);
  }


  // Close the given gate number
  void GateServos::closegate(int gatenum)
  {
    DPRINT("CLOSING GATE #");
    DPRINT(gatenum);
    DPRINTLN("");
    
    digitalWrite(ledpin[gatenum], LOW);
    
    // Only control the servo if the pin is valid (not -1)
    if (servopin[gatenum] != -1) {
      myservo.attach(servopin[gatenum]);  // attaches the servo
      myservo.write(maxservo[gatenum]); //close gate
      delay(closedelay); // wait for gate to close
      myservo.detach();
      DPRINTLN("CLOSED GATE");
    } else {
      DPRINTLN("SKIPPED SERVO (PIN DISABLED)");
      delay(closedelay); // still delay for consistency
    }
  }


  // Debug function to test servo by opening and closing it given a pin number
  void GateServos::testServo(int servopin)
  {  
    DPRINT("TESTING SERVO #");
    DPRINTLN(servopin);
    myservo.attach(servopin);  // attaches the servo
    myservo.write(255); 
    delay(2000);
    DPRINTLN("Set to 255");
    myservo.write(0);
    delay(2000);
    DPRINTLN("Set to 0");
    myservo.detach();
  }

  // Initialize gates and close them all
  //
  void GateServos::initializeGates()
  {
    //testServo(12);
      // close all gates one by one
    for (int thisgate = 0; thisgate < num_gates; thisgate++)
    {
     // Always set up the LED pin
     pinMode(ledpin[thisgate], OUTPUT);
     digitalWrite(ledpin[thisgate], HIGH);
     
     // Only control the servo if the pin is valid (not -1)
     if (servopin[thisgate] != -1) {
       DPRINT("Initializing gate #");
       DPRINT(thisgate);
       DPRINT(" on pin ");
       DPRINTLN(servopin[thisgate]);
       
       myservo.attach(servopin[thisgate]);  // attaches the servo
       myservo.write(maxservo[thisgate]); //close gate
       delay(closedelay); // wait for gate to close
       myservo.detach();
     } else {
       DPRINT("Skipping disabled gate #");
       DPRINTLN(thisgate);
       delay(100); // short delay for consistency
     }
     
     digitalWrite(ledpin[thisgate], LOW);
    }
  }

  // Turn LED on for given gate number
  void GateServos::ledon(int gatenum)
  {
    digitalWrite(ledpin[gatenum], HIGH);
  }

  // Turn LED off for given gate number
  void GateServos::ledoff(int gatenum)
  {
    digitalWrite(ledpin[gatenum], LOW);
  }

  // User has pushed button to manually open given gate
  void GateServos::ManuallyOpenGate(int curselectedgate)
  {
      if (curselectedgate == num_gates)
      {
        // Close all gates option
        if (curopengate > -1) {
          closegate(curopengate);
        }
        curopengate=-1;
        curselectedgate=-1;
      }
      else
      {
        // First close the currently open gate (if any)
        if (curopengate > -1)
        {
          DPRINT("Button Closing Gate= ");
          DPRINTLN(curopengate);
          closegate(curopengate);
        }
        
        // Then open the selected gate
        curopengate = curselectedgate;
        DPRINT("Button Opening Gate= ");
        DPRINTLN(curopengate);
        
        // Even if the servo pin is disabled (-1), we still update the state
        // and control the LED to maintain the user-facing gate numbering
        opengate(curopengate);
      }
  }
  
  // return index of first open gate or -1 for none
  int GateServos::firstgateopen()
  {
    for (int curgate = 0; curgate < num_gates; curgate++) {
      // Only consider gates with valid servo pins (not -1)
      if (gateopen[curgate]) {
        if (servopin[curgate] != -1) {
          return curgate;
        } else {
          DPRINT("Skipping disabled gate #");
          DPRINTLN(curgate);
        }
      }
    }
  
    return -1;
  }
