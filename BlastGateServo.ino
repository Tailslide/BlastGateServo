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
 *   
 */
#include <Servo.h>

//#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
#endif

const int closedelay = 1000; // how long it takes a gate to close
const int opendelay = 1000; // how long after last button push to open gate
const int buttonPin = 13;     // the number of the pushbutton pin
const int numgates = 4;
const int servopin[] = {11,10,8,9}; // (My gates) 9 = table saw gate, 8 = sander gate, 10 = hand tool gate, 11= miter saw gate
const int ledpin[] = {3,4,5,6}; // LED pins
const int maxservo[] = {180,166,166,166}; // close positions
const int minservo[] = {100,100,0,0}; // open positions
const bool hasbutton = true;
 
int buttonState = 0;         // variable for reading the pushbutton status
int lastbuttonpushms = 0;

bool gateopen[] = {false, false, false, false};
bool gateclosed[] = {true, true, true, true}; 

int curopengate = -1;
int curselectedgate = -1;
bool waitingforbuttonrelease = false;

Servo myservo;  // create servo object to control a servo
             // a maximum of eight servo objects can be created

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
  
  // close all gates one by one
  for (int thisgate = 0; thisgate < numgates; thisgate++)
  {
   pinMode(ledpin[thisgate], OUTPUT);
   digitalWrite(ledpin[thisgate], HIGH);
   myservo.attach(servopin[thisgate]);  // attaches the servo
   myservo.write(maxservo[thisgate]); //close gate
   delay(closedelay); // wait for gate to close
   myservo.detach();
   digitalWrite(ledpin[thisgate], LOW);
  }
  pinMode(LED_BUILTIN, OUTPUT);
   // initialize the pushbutton pin as an input:
  if (hasbutton) pinMode(buttonPin, INPUT);
}

void closegate(int gatenum)
{
    digitalWrite(ledpin[gatenum], LOW);
    myservo.attach(servopin[gatenum]);  // attaches the servo
    myservo.write(maxservo[gatenum]); //close gate
    delay(closedelay); // wait for gate to close
    myservo.detach();
}

void opengate(int gatenum)
{
    curopengate = gatenum;
    digitalWrite(ledpin[gatenum], HIGH);
    myservo.attach(servopin[gatenum]);  // attaches the servo
    myservo.write(minservo[gatenum]); //open gate
    delay(closedelay); // wait for gate to close
    myservo.detach();
}

void loop() 
{

    // read the state of the pushbutton value:
  if (hasbutton) buttonState = digitalRead(buttonPin);
  if (hasbutton && buttonState == HIGH)
  {
    if (! waitingforbuttonrelease)
    {
      lastbuttonpushms = 0;
      DPRINT("Button Pushed, curselectedgate= ");  
      DPRINTLN(curselectedgate);
      digitalWrite(ledpin[curselectedgate], LOW);
      curselectedgate++;
      if (curselectedgate == numgates) 
        curselectedgate = -1;
      else
        digitalWrite(ledpin[curselectedgate], HIGH);                
      waitingforbuttonrelease= true;
    }
  }
  
  if (hasbutton && buttonState == LOW)
  {
    if (waitingforbuttonrelease)
    {
       waitingforbuttonrelease=false;
       lastbuttonpushms = 0;
    }
  
    if (lastbuttonpushms > opendelay)
    {    
        if (curselectedgate == numgates)
        {
          closegate(curopengate);
          curopengate=-1;
          curselectedgate=-1;
        }
        else
        {
          if (curopengate > -1) 
          {
            DPRINT("Closing Gate= ");  
            DPRINTLN(curopengate);
            closegate(curopengate);
          }      
          curopengate = curselectedgate;   
          DPRINT("Opening Gate= ");  
          DPRINTLN(curopengate);
          opengate(curopengate);      
        }
     }
   }

   delay(50);
   if (curselectedgate == curopengate) 
      lastbuttonpushms = 0;
   else
      lastbuttonpushms +=50;
   
}
