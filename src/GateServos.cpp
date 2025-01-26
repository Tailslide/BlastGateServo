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
      myservo.attach(servopin[gatenum]);  // attaches the servo
      myservo.write(minservo[gatenum]); //open gate
      delay(opendelay); // wait for gate to close
      myservo.detach();
      DPRINTLN("OPENED GATE");
      //if (servopin[gatenum] ==12) testServo(12);
  }


  // Close the given gate number
  void GateServos::closegate(int gatenum)
  {
    DPRINT("CLOSING GATE #");
    DPRINT(gatenum);
    DPRINTLN("");
    digitalWrite(ledpin[gatenum], LOW);
    myservo.attach(servopin[gatenum]);  // attaches the servo
    myservo.write(maxservo[gatenum]); //close gate
    delay(closedelay); // wait for gate to close
    myservo.detach();
    DPRINTLN("CLOSED GATE");
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
     pinMode(ledpin[thisgate], OUTPUT);
     digitalWrite(ledpin[thisgate], HIGH);
     myservo.attach(servopin[thisgate]);  // attaches the servo
     myservo.write(maxservo[thisgate]); //close gate
     delay(closedelay); // wait for gate to close
     myservo.detach();
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
        closegate(curopengate);
        curopengate=-1;
        curselectedgate=-1;
      }
      else
      {
        if (curopengate > -1) 
        {
          DPRINT("Button Closing Gate= ");  
          DPRINTLN(curopengate);
          closegate(curopengate);
        }      
        curopengate = curselectedgate;   
        DPRINT("Button Opening Gate= ");  
        DPRINTLN(curopengate);
        opengate(curopengate);      
      }
  }
  
  // return index of first open gate or -1 for none
  int GateServos::firstgateopen()
  {
    for (int curgate = 0; curgate < num_gates; curgate++)
      if (gateopen[curgate]) return curgate;
  
    return -1;
  }
