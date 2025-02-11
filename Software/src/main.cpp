#include <Arduino.h>
#include "SevSeg.h"


SevSeg sevseg; //Instantiate a seven segment controller object

int leistung_prozent = 0;

int pot_pin = A0;
int button1_pin = A1;
int button2_pin = A2;
int pump_out = A3; 

bool button1_debounce = 0;
bool button2_debounce = 0;
bool button1_state = 0;
bool button2_state = 0;
bool pump_state = 0;

unsigned long timer = 0;
int timeercount = 0;
int updatecount = 0;




// put function declarations here:


/**
  * URL: https://dbuezas.github.io/arduino-web-timers/#mcu=ATMEGA328P&timer=2&topValue=OCR2A&CompareOutputModeA=disconnect&OCR2A=73&timerMode=CTC
  * Mode     : CTC
  * Period   : 4.625 us
  * Frequency: 216.21622 kHz
  * Outputs  : 
  *  - B3: 50.00%, toggle
  */
void setup(){
  noInterrupts();
  TCCR2A = 
    1 << COM2A0 |
    1 << WGM21;
  TCCR2B = 
    1 << CS20;
  DDRB = 
    1 << DDB3;
  OCR2A = 73;
  interrupts();



  byte numDigits = 3;
  byte digitPins[] = {4, 3, 2};
  byte segmentPins[] = {5, 6, 7, 8, 9, 10, 12, 13};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected
  
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90);


  pinMode(pot_pin,INPUT);
  pinMode(button1_pin, INPUT_PULLUP);
  pinMode(button2_pin, INPUT_PULLUP);
  pinMode(pump_out,OUTPUT);

}


void loop() { 

  if (micros() - timer >= 10) {
    timer = micros();
    timeercount ++;
    
    if (timeercount >= 100) { // Reset to 0 after counting for 1000 seconds.
      timeercount=0;
      updatecount ++;
      
      if (updatecount > 30){
        updatecount = 0;
        leistung_prozent = map(analogRead(pot_pin),0,1023,0,100);
        sevseg.setNumber(leistung_prozent, 0);

        if (digitalRead(button1_pin)==LOW && button1_debounce == true){
          button1_state = true;

        }
        else if (digitalRead(button1_pin)==LOW){
          button1_debounce = true;
        }
        else if (digitalRead(button1_pin)==HIGH && button1_state == true){
          button1_state = false;
          button1_debounce = false;
          pump_state = !pump_state;
          digitalWrite(pump_out, pump_state);
          }
        else {
          button1_debounce = false;
        }

      }
      
    }
    if (timeercount < leistung_prozent){
      TCCR2A = 1 << COM2A0 | 1 << WGM21;
    }
    else{
      TCCR2A = 0 << COM2A0 | 1 << WGM21;
    }

  }

  



  
  sevseg.refreshDisplay(); // Must run repeatedly




}

// put function definitions here:
