#include "main.h"
#include "neopixel.h"
#include <IntervalTimer.h>


// Create an IntervalTimer object
IntervalTimer timer;
void shifterCallback(); 

// int const button = 6;
// int const shiftUp = 43;
// int const shiftDown = 42;
int const button3 = 44;
int const button4 = 45;
int const button5 = 6;
int const button6 = 9;

void setup() {
  // pinMode(shiftUp, INPUT_PULLUP);
  // pinMode(shiftDown, INPUT_PULLUP);
  // pinMode(button3,INPUT_PULLUP);
  // pinMode(button4,INPUT_PULLUP);
  // pinMode(button5,INPUT_PULLUP);
  pinMode(button6,INPUT_PULLUP);

  

  //Inits buttons
  // pinMode(button, INPUT_PULLUP);
  
  Serial.begin(9600);
  //Starts Nextion
  Serial.println("Starting nextion interface");
  NextionInterface::init(); // Creates Serial Port to Display
  Serial.println("Nextion interface initialized.");

  NextionInterface::switchToDriver();
  

  while(NextionInterface::getCurrentPage() != page::DRIVER){
      Serial.println("NOT Breaking");
  }
  //Start Can
  Serial.println("NOT Breaking");
  CanInterface::init();


  //Starts Rev Lights
  RevLights::begin(75, true, 9600);
  //Switches to the driver screen

  // timer.begin(lapTimeButton, 1000);
  timer.begin(shifterCallback, 20000);
}

void loop() {
  //Updates Can
  CanInterface::task();

}

void lapTimeButton(){
  
    // if(digitalRead(button) == 0){
    
    // CanInterface::lapTime(true);
    // }
    // else{
    //   CanInterface::lapTime(false);
    // }
    
  
}

void shifterCallback() { // This function will be called every 20 milliseconds (50 Hz)
  static bool button3State = false;

  if(digitalRead(button6) ==1 && button3State == false){
    button3State = true;
    
  }
  else if (digitalRead(button6) == 0 && button3State == true){
    button3State = false;
  }

  CanInterface::send_shift(false, false,button3State);

}