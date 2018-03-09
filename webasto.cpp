#include <Arduino.h>
//#include <HardwareSerial.h>
#include <SoftwareSerial.h>
//#include <avr/io.h>
//#include <util/delay.h>
 
#define BLINK_DELAY_MS 400
#define BAUD_RATE 300

#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
#endif


//#define setbit(port, bit) (port) |= (1 << (bit))
//#define clearbit(port, bit) (port) &= ~(1 << (bit))
 
//can only be pins 8-13 because prtmapping is hardcoded to PORTB 
int8_t rxPin=10;
uint8_t txPin=11;

// set up a new serial port
SoftwareSerial mySerial =  SoftwareSerial(rxPin,txPin);

void init_board() {
 
 //call arduino init()
 init();
 mySerial.begin(BAUD_RATE);
#ifdef DEBUG
 Serial.begin(9600);
 DPRINTLN("Hello world");
 delay(2000);
#endif
}


int main (void)
{
//Init arduino
init_board();

// main loop
while(1) {
  delay(BLINK_DELAY_MS);
  mySerial.println("Hello World - SoftwareSerial");
  DPRINTLN("bajs bajs");

 }
}
