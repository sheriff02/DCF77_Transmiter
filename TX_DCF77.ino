/*                       *** Transmitter DCF77 ***
____________________________________________________________________________________

            77.5 KHz oscillator, ON / OFF controlled (TX DCF77 pulses)
                             >>> ATmega328P <<<
                 Written by: J_RPM translated by Sheriff02
                http://j-rpm.com/2019/04/en-hora-con-dcf77/
                               April 2019
____________________________________________________________________________________
*/

#include <PWM.h> 
const int inData = 2;                     // DCF77 data input (0 = OFF - Disconnects the carrier)              
const int outLed = 4;                     // Carrier LED
const int iniLed = 5;                     // Minute LED 
const int zeroLed = 6;                    // LED Bit = 0 
const int oneLed = 7;                     // LED Bit = 1 
const int outDCF = 9;                     // Carrier output 77.5 KHz
int32_t DCF_freq = 77500;                 // Real = 77609 Hz (Jumps: 76864 ... 77609)
boolean flank = false;                    // State change detection
int i=0;                                  // Measurement of the interval, to display the LED data
int j=0;                                  // TimeOut LED on

void setup() {
  pinMode(inData, INPUT);                 // DCF77 data input
  digitalWrite(inData, HIGH);             // Pull-Up
  pinMode(outDCF, OUTPUT);      
  pinMode(outLed, OUTPUT);      
  pinMode(iniLed, OUTPUT);      
  pinMode(zeroLed, OUTPUT);      
  pinMode(oneLed, OUTPUT);      
  digitalWrite(outLed, HIGH);             // Turn off DCF carrier LED indicator                                                                           
  Serial.begin(9600); 
  Serial.println("------------------------");
  Serial.println("***  Interval M/0/1  ***");
  Serial.println("------------------------");
  InitTimersSafe();                       // Initialize all timers except 0
}

void loop(){
  SetPinFrequencySafe(outDCF, DCF_freq);  // Allows frequencies between 1 Hz and 2 MHz (16 bit)
  if(digitalRead (inData)==HIGH){
    pwmWriteHR(outDCF, 32768);            // 50% cycle (16-bit 1/2 of 65536 = 32768)
    digitalWrite(outLed, LOW);            // Led ON
    i=i+1;
    j=j+1;
    if (j>6000) {                         // LED TimeOut
      turnoffLed();                                            
      j=0;
    }
    if (i>10000) {
      i=0;                                // Reset the counter if data is lost
    }
    if (flank == true) {
      flank=false;
      showLed();      
    }
  } else {
    pwmWriteHR(outDCF, 0);                // Output = 0 (Without DCF77 carrier)   
    digitalWrite(outLed, HIGH);           // Turn off DCF carrier LED indicator                                                                          
    flank = true;
  }
}
///////////////////////////////////////
//////////// Subroutines //////////////
///////////////////////////////////////
void showLed() {
  Serial.print("Interval=");
  Serial.println(i);
  if (i<2000) {                           // >>> Minute start 
    digitalWrite(iniLed, LOW);            // Start LED ON
    digitalWrite(zeroLed, HIGH);          // Zero LED OFF
    digitalWrite(oneLed, HIGH);           // One LED OFF
  }else if(i<4000) {                      // >>> Bit = 1
    digitalWrite(oneLed, LOW);            // One LED ON
    digitalWrite(iniLed, HIGH);           // Start LED OFF
    digitalWrite(zeroLed, HIGH);          // Zero Led OFF
  }else {                                 // >>> Bit = 0
    digitalWrite(zeroLed, LOW);           // Zero LED ON                                                                             
    digitalWrite(iniLed, HIGH);           // Start LED OFF
    digitalWrite(oneLed, HIGH);           // One LED OFF
  }
  i=0;                                    // Reset the counter
}
///////////////////////////////////////
void turnoffLed() {
  digitalWrite(iniLed, HIGH);             // Turn off LED of the minute start bit                                                                             
  digitalWrite(zeroLed, HIGH);            // Turn off LED Bit = 0                                                                           
  digitalWrite(oneLed, HIGH);             // Turn off LED Bit = 1                                                                             
}
////////////////////// END ///////////////////////////////
