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
      DPRINT(gatenum + 1); // Display as 1-based
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
        // Ensure the servo is properly attached
        myservo.attach(servopin[gatenum]);  // attaches the servo
        
        // Debug the servo position
        DPRINT("Setting servo to position: ");
        DPRINTLN(minservo[gatenum]);
        
        // Move the servo to open position
        myservo.write(minservo[gatenum]); //open gate
        
        // Wait for gate to open
        delay(opendelay);
        
        // Detach the servo to prevent jitter
        myservo.detach();
        
        DPRINTLN("OPENED GATE");
      } else {
        DPRINTLN("SKIPPED SERVO (PIN DISABLED)");
        delay(opendelay); // still delay for consistency
      }
  }


  // Close the given gate number
  void GateServos::closegate(int gatenum)
  {
    DPRINT("CLOSING GATE #");
    DPRINT(gatenum + 1); // Display as 1-based
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
       DPRINT(thisgate + 1); // Display as 1-based
       DPRINT(" on pin ");
       DPRINTLN(servopin[thisgate]);
       
       myservo.attach(servopin[thisgate]);  // attaches the servo
       myservo.write(maxservo[thisgate]); //close gate
       delay(closedelay); // wait for gate to close
       myservo.detach();
     } else {
       DPRINT("Skipping disabled gate #");
       DPRINTLN(thisgate + 1); // Display as 1-based
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
      DPRINTLN("ManuallyOpenGate called");
      
      if (curselectedgate == -1)
      {
        // Close all gates option
        DPRINTLN("Closing all gates");
        if (curopengate > -1) {
          DPRINT("Closing gate #");
          DPRINTLN(curopengate + 1);
          closegate(curopengate);
        }
        curopengate = -1;
      }
      else
      {
        // First close the currently open gate (if any)
        if (curopengate > -1)
        {
          DPRINT("Closing gate #");
          DPRINTLN(curopengate + 1);
          closegate(curopengate);
        }
        
        // Then open the selected gate
        curopengate = curselectedgate;
        DPRINT("Opening gate #");
        DPRINTLN(curopengate + 1);
        
        // Debug the servo pin
        DPRINT("Using servo pin: ");
        DPRINTLN(servopin[curopengate]);
        
        // Debug the servo position
        DPRINT("Target position: ");
        DPRINTLN(minservo[curopengate]);
        
        // Even if the servo pin is disabled (-1), we still update the state
        // and control the LED to maintain the user-facing gate numbering
        opengate(curopengate);
      }
      
      DPRINTLN("ManuallyOpenGate completed");
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
          DPRINTLN(curgate + 1); // Display as 1-based
        }
      }
    }
  
    return -1;
  }
  
  // Check if a gate is disabled (servo pin = -1)
  bool GateServos::isGateDisabled(int gatenum)
  {
    if (gatenum < 0 || gatenum >= num_gates) {
      return false; // All gates closed option (-1) is not considered disabled
    }
    return servopin[gatenum] == -1;
  }
