#include "Arduino.h"
#include "Debug.h"
#include "Configuration.h"
#include "AcSensors.h"

const float AcSensors::acsensorsentitivity = AC_SENSOR_SENSITIVITY;

  AcSensors::AcSensors()
  {
    DPRINTLN("AC Sensors object created");
  }
  
  float AcSensors::GetOffReading(int sensor) {
      if (sensor >= 0 && sensor < num_ac_sensors) {
          return offReadings[sensor];
      }
      return 0;
  }
  
  float AcSensors::GetAvgReading(int sensor) {
      return AvgSensorReading(sensor);
  }
  
  //////////////////////////////////////////////////////////////////////
  // InitializeSensors()
  //
  // Initialize sensors and read a baseline sensor reading with tools off
  //
  //////////////////////////////////////////////////////////////////////
  void AcSensors::InitializeSensors()
  {
      DPRINTLN("Initializing AC sensors...");
      DPRINT("Number of sensors: "); DPRINTLN(num_ac_sensors);
      
      for (int x = 0; x < num_ac_sensors && x < NUM_AC_SENSORS; x++) {
          DPRINT("Setting up LED pin "); DPRINT(ledpin[x]); DPRINTLN(" as OUTPUT");
          pinMode(ledpin[x], OUTPUT);
      }
      
      DPRINTLN("Getting baseline sensor readings...");
      //getAvgOffSensorReadings();
  
      getMaxOffSensorReadings();
      DPRINTLN("AC sensor initialization complete");
  }


  //////////////////////////////////////////////////////////////////////
  // AvgSensorReading(int forsensor)
  //
  // returns an average of the last X sensors readings for given sensor
  //
  //////////////////////////////////////////////////////////////////////
  float AcSensors::AvgSensorReading(int forsensor)
  {
    int res = 0;
    for (int x = 0; x < avg_readings; x++)
    {
      res = res + recentReadings[forsensor][x];
    }
    return (float)res / (float)avg_readings;
  }

  //////////////////////////////////////////////////////////////////////
  // getMaxOffSensorReadings()
  //
  // Poll for NUM_OFF_MAX_SAMPLES ms to determine maximum 'off' sensor
  // reading for this sensor
  //
  //////////////////////////////////////////////////////////////////////
  void AcSensors::getMaxOffSensorReadings()
  {
    for (int x = 0; x < num_ac_sensors && x < NUM_AC_SENSORS; x++)
    {
        int maxsensorval = 0;
        for (long y = 0; y < numoffmaxsamples; y++)
        {      
          int sensorval = analogRead(sensorPins[x]);
          if (sensorval > maxsensorval) maxsensorval = sensorval;
          delay(1);
        }
        offReadings[x] = maxsensorval;
        DPRINT("OFF READING: ");
        DPRINTLN(offReadings[x]);
    }
  }

  //////////////////////////////////////////////////////////////////////
  // ReadSensors()
  //
  // Read values for AC current sensors and add to list of values we will average
  //
  //////////////////////////////////////////////////////////////////////  
  void AcSensors::ReadSensors()
  {
    curreadingindex ++;
    if (curreadingindex >= avg_readings) curreadingindex =0;
    
      // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    for (int cursensor=0; cursensor < num_ac_sensors && cursensor < NUM_AC_SENSORS; cursensor++)
    {
       int sensorValue = analogRead(sensorPins[cursensor]);
       //float voltage = sensorValue * (5.0 / 1023.0);
       
       //DPRINT("Reading Sensor ");  
       //DPRINTLN(cursensor);
       recentReadings[cursensor][curreadingindex] = sensorValue;
    }
  
  }

    
  //////////////////////////////////////////////////////////////////////
  // DisplayMeter()
  //
  // Use LEDs to display a meter for positioning AC sensor clamps. 
  // Turn on a device like a tool or a space heater on the cable the sensor is on.
  // Rotate the sesnsor around the cable until the light for that 
  // sensor is flashing as fast as possible (or has become solidly lit).
  //
  //////////////////////////////////////////////////////////////////////  
  void AcSensors::DisplayMeter()
  {
      DPRINTLN("\n--- Meter Mode Readings ---");
      for (int cursensor= 0; cursensor < num_ac_sensors && cursensor < NUM_AC_SENSORS; cursensor++)
      {
        DPRINT("Sensor #"); DPRINT(cursensor + 1); DPRINT(": ");
        int avgthissensor =  AvgSensorReading(cursensor);
        //float percent = avgthissensor / 4.5;
        // Calculate the signal strength relative to baseline
        float delta = avgthissensor - offReadings[cursensor];
        
        // Only consider positive changes from baseline
        float percent = 0;
        if (delta > 0) {
            percent = delta / (1023 - offReadings[cursensor]);
            if (percent > 1) percent = 1;
        }
        
        // Debug raw values
        DPRINT(" Raw: "); DPRINT(avgthissensor);
        DPRINT(" Baseline: "); DPRINT(offReadings[cursensor]);
        DPRINT(" Delta: "); DPRINT(delta);
        
        // Calculate blink length based on signal strength
        int blinklen = maxblinklen;  // Default to slow blink for no signal
        if (delta > 0) {
          blinklen = maxblinklen * (1 - (delta / 1023.0));
          if (blinklen < 10) blinklen = 10;  // Minimum blink time to prevent flicker
        }
        
        // Increment blink timer
        blinktimers[cursensor]++;
        
        // Toggle LED state when timer reaches blink length
        if (blinktimers[cursensor] >= blinklen) {
          blinktimers[cursensor] = 0;
          blinkon[cursensor] = !blinkon[cursensor];
          digitalWrite(ledpin[cursensor], blinkon[cursensor] ? HIGH : LOW);
        }
  
        // print out the value you read:
        DPRINT(percent);
        DPRINT("    " );
        DPRINT(avgthissensor);
        DPRINT(" BlinkLen:");
        DPRINT(blinklen);
        DPRINT(" BlinkTimer:");
        DPRINT(blinktimers[cursensor]);
        DPRINTLN("");
        #ifdef DEBUG
        //displayaverages(cursensor);
        #endif
              
        if (blinktimers[cursensor] > blinklen)
        {
          blinktimers[cursensor] = 0;
          if (blinkon[cursensor])
          {
           digitalWrite(ledpin[cursensor], LOW);
           DPRINT("LED OFF " );
           DPRINTLN(cursensor);
           blinkon[cursensor]=false;
          }
          else
          {
           digitalWrite(ledpin[cursensor], HIGH);
           DPRINT("LED ON" );
           DPRINTLN(cursensor);
           blinkon[cursensor] = true;
          }
        }
      }
  }
  

  //////////////////////////////////////////////////////////////////////
  // getAvgOffSensorReadings()
  //
  // Determine average 'off' reading for each sensor.  
  // Currently unused but might be useful if you have a tool that isn't 
  // drawing enough power to trigger the sensor using max values.
  //
  //////////////////////////////////////////////////////////////////////
  void AcSensors::getAvgOffSensorReadings()
  {
    for (int x = 0; x < num_ac_sensors && x < NUM_AC_SENSORS; x++)
    {
        long totalsensorval = 0;
        for (int y = 0; y < numoffsamples; y++)
        {      
          totalsensorval += analogRead(sensorPins[x]);
          delay(100);
        }
        offReadings[x] = (float)totalsensorval/ (float)numoffsamples;
        DPRINT("OFF READING: ");
        DPRINTLN(offReadings[x]);
    }
  }

  //////////////////////////////////////////////////////////////////////
  // Triggered(int forsensor)
  //
  // Returns true if the given AC current sensor number is triggered
  //////////////////////////////////////////////////////////////////////  
  bool AcSensors::Triggered(int forsensor)
  {
    return (AvgSensorReading(forsensor) > ((float)offReadings[forsensor]  * acsensorsentitivity));
  }


  //////////////////////////////////////////////////////////////////////
  // displayaverages(int cursensor)
  //
  // Debugging function to display values polled for given sensor
  //////////////////////////////////////////////////////////////////////
  void AcSensors::displayaverages(int cursensor)
  {
      Serial.print( recentReadings[cursensor][0]);
      Serial.print(" ");
      Serial.print( recentReadings[cursensor][1]);
      Serial.print(" ");
      Serial.print( recentReadings[cursensor][2]);
      Serial.print(" ");
      Serial.print( recentReadings[cursensor][3]);
      Serial.print(" ");
      Serial.print( recentReadings[cursensor][4]);
      Serial.print(" ");
      Serial.print( recentReadings[cursensor][5]);
      Serial.print(" ");
      Serial.print( recentReadings[cursensor][6]);
      Serial.print(" ");
      Serial.print( recentReadings[cursensor][7]);
      Serial.print(" ");
      Serial.print( recentReadings[cursensor][8]);
      Serial.print(" ");
      Serial.print( recentReadings[cursensor][9]);
      Serial.print(" ");
      Serial.print( recentReadings[cursensor][0]);
      Serial.println(" ");
  }
  
    
