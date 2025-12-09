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
    
    // Initialize operation times buffer
    for (int i = 0; i < maxOpsPerMinute; i++) {
      operationTimes[i] = 0;
    }
    
    // Initialize queued operations
    for (int i = 0; i < 8; i++) {
      queuedOps[i].pending = false;
      queuedOps[i].gatenum = -1;
      queuedOps[i].isOpen = false;
      queuedOps[i].requestTime = 0;
    }
  }

  //////////////////////////////////////////////////////////////////////
  // checkOperationAllowed(int gatenum)
  //
  // Check if a servo operation is allowed based on flutter protection rules
  // Returns true if operation can execute immediately
  // Returns false if operation should be queued or blocked
  //////////////////////////////////////////////////////////////////////
  bool GateServos::checkOperationAllowed(int gatenum)
  {
    // Don't allow operations if in error state
    if (errorState) {
      DPRINTLN("Operation blocked: System in error state");
      return false;
    }
    
    unsigned long currentTime = millis();
    
    // Check rate limiting first - count operations in last minute
    unsigned long oneMinuteAgo = currentTime - 60000;
    int recentOps = 0;
    for (int i = 0; i < maxOpsPerMinute; i++) {
      if (operationTimes[i] > oneMinuteAgo) {
        recentOps++;
      }
    }
    
    if (recentOps >= maxOpsPerMinute) {
      // Too many operations - enter error state
      errorState = true;
      DPRINTLN("CRITICAL ERROR: Too many servo operations detected!");
      DPRINT("Operations in last minute: ");
      DPRINTLN(recentOps);
      DPRINTLN("System entering error state - restart required");
      return false;
    }
    
    // Check minimum interval between operations on same gate
    if (currentTime - lastOperationTime[gatenum] < minServoInterval) {
      DPRINT("Operation too soon for gate #");
      DPRINT(gatenum + 1);
      DPRINT(" (");
      DPRINT(currentTime - lastOperationTime[gatenum]);
      DPRINT("ms < ");
      DPRINT(minServoInterval);
      DPRINTLN("ms) - will be queued");
      return false;
    }
    
    return true;
  }

  //////////////////////////////////////////////////////////////////////
  // recordOperation(int gatenum)
  //
  // Record a servo operation for rate limiting tracking
  //////////////////////////////////////////////////////////////////////
  void GateServos::recordOperation(int gatenum)
  {
    unsigned long currentTime = millis();
    lastOperationTime[gatenum] = currentTime;
    
    // Add to circular buffer
    operationTimes[operationIndex] = currentTime;
    operationIndex = (operationIndex + 1) % maxOpsPerMinute;
  }

  //////////////////////////////////////////////////////////////////////
  // isInErrorState()
  //
  // Check if the system is in error state
  //////////////////////////////////////////////////////////////////////
  bool GateServos::isInErrorState()
  {
    return errorState;
  }

  //////////////////////////////////////////////////////////////////////
  // queueOperation(int gatenum, bool isOpen)
  //
  // Queue an operation for delayed execution when minimum interval expires
  //////////////////////////////////////////////////////////////////////
  void GateServos::queueOperation(int gatenum, bool isOpen)
  {
    if (gatenum < 0 || gatenum >= 8) return;
    
    queuedOps[gatenum].gatenum = gatenum;
    queuedOps[gatenum].isOpen = isOpen;
    queuedOps[gatenum].requestTime = millis();
    queuedOps[gatenum].pending = true;
    
    DPRINT("Queued ");
    DPRINT(isOpen ? "OPEN" : "CLOSE");
    DPRINT(" operation for gate #");
    DPRINTLN(gatenum + 1);
  }

  //////////////////////////////////////////////////////////////////////
  // processQueuedOperations()
  //
  // Process any pending queued operations that can now execute
  // Should be called regularly from main loop
  //////////////////////////////////////////////////////////////////////
  void GateServos::processQueuedOperations()
  {
    if (errorState) return; // Don't process queue in error state
    
    unsigned long currentTime = millis();
    
    for (int i = 0; i < 8; i++) {
      if (!queuedOps[i].pending) continue;
      
      // Check if enough time has passed since last operation on this gate
      if (currentTime - lastOperationTime[i] >= minServoInterval) {
        DPRINT("Executing queued ");
        DPRINT(queuedOps[i].isOpen ? "OPEN" : "CLOSE");
        DPRINT(" for gate #");
        DPRINTLN(i + 1);
        
        // Clear the queue entry first to avoid recursion
        queuedOps[i].pending = false;
        
        // Execute the operation
        if (queuedOps[i].isOpen) {
          opengate(i);
        } else {
          closegate(i);
        }
      }
    }
  }

  // Open the given gate number
  //
  void GateServos::opengate(int gatenum)
  {
      // Check if operation is allowed (flutter protection)
      if (!checkOperationAllowed(gatenum)) {
        // Queue the operation for later execution
        queueOperation(gatenum, true);
        return;
      }
      
      DPRINT("OPENING GATE #");
      DPRINT(gatenum + 1); // Display as 1-based
      DPRINT(" SERVO PIN:");
      DPRINT(servopin[gatenum]);
      
      // Determine the correct position based on gate orientation
      int openPosition;
      if (gateClosedAtMax[gatenum]) {
        // Normal orientation: min = open, max = closed
        openPosition = minservo[gatenum];
      } else {
        // Inverted orientation: max = open, min = closed
        openPosition = maxservo[gatenum];
      }
      
      DPRINT(" VALUE:");
      DPRINT(openPosition);
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
        DPRINTLN(openPosition);
        
        // Move the servo to open position
        myservo.write(openPosition); //open gate
        
        // Wait for gate to open
        delay(opendelay);
        
        // Detach the servo to prevent jitter
        myservo.detach();
        
        DPRINTLN("OPENED GATE");
        
        // Record this operation for flutter protection
        recordOperation(gatenum);
      } else {
        DPRINTLN("SKIPPED SERVO (PIN DISABLED)");
        delay(opendelay); // still delay for consistency
      }
  }


  // Close the given gate number
  void GateServos::closegate(int gatenum)
  {
    // Check if operation is allowed (flutter protection)
    if (!checkOperationAllowed(gatenum)) {
      // Queue the operation for later execution
      queueOperation(gatenum, false);
      return;
    }
    
    DPRINT("CLOSING GATE #");
    DPRINT(gatenum + 1); // Display as 1-based
    
    // Determine the correct position based on gate orientation
    int closePosition;
    if (gateClosedAtMax[gatenum]) {
      // Normal orientation: max = closed, min = open
      closePosition = maxservo[gatenum];
    } else {
      // Inverted orientation: min = closed, max = open
      closePosition = minservo[gatenum];
    }
    
    DPRINT(" VALUE:");
    DPRINTLN(closePosition);
    
    digitalWrite(ledpin[gatenum], LOW);
    
    // Only control the servo if the pin is valid (not -1)
    if (servopin[gatenum] != -1) {
      myservo.attach(servopin[gatenum]);  // attaches the servo
      myservo.write(closePosition); //close gate
      delay(closedelay); // wait for gate to close
      myservo.detach();
      DPRINTLN("CLOSED GATE");
      
      // Record this operation for flutter protection
      recordOperation(gatenum);
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
    for (int thisgate = 0; thisgate < num_gates && thisgate < 8; thisgate++)
    {
     // Always set up the LED pin
     pinMode(ledpin[thisgate], OUTPUT);
     digitalWrite(ledpin[thisgate], HIGH);
     
     // Determine the correct position based on gate orientation
     int closePosition;
     if (gateClosedAtMax[thisgate]) {
       // Normal orientation: max = closed, min = open
       closePosition = maxservo[thisgate];
     } else {
       // Inverted orientation: min = closed, max = open
       closePosition = minservo[thisgate];
     }
     
     // Only control the servo if the pin is valid (not -1)
     if (servopin[thisgate] != -1) {
       DPRINT("Initializing gate #");
       DPRINT(thisgate + 1); // Display as 1-based
       DPRINT(" on pin ");
       DPRINT(servopin[thisgate]);
       DPRINT(" to position ");
       DPRINTLN(closePosition);
       
       myservo.attach(servopin[thisgate]);  // attaches the servo
       myservo.write(closePosition); //close gate
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
        
        // Determine the correct position based on gate orientation
        int openPosition;
        if (gateClosedAtMax[curopengate]) {
          // Normal orientation: min = open, max = closed
          openPosition = minservo[curopengate];
        } else {
          // Inverted orientation: max = open, min = closed
          openPosition = maxservo[curopengate];
        }
        
        // Debug the servo position
        DPRINT("Target position: ");
        DPRINTLN(openPosition);
        
        // Even if the servo pin is disabled (-1), we still update the state
        // and control the LED to maintain the user-facing gate numbering
        opengate(curopengate);
      }
      
      DPRINTLN("ManuallyOpenGate completed");
  }
  
  // return index of first open gate or -1 for none
  int GateServos::firstgateopen()
  {
    for (int curgate = 0; curgate < num_gates && curgate < 8; curgate++) {
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
