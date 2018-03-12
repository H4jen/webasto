#include <Arduino.h>
//#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
//#include <avr/io.h>
//#include <util/delay.h>

#define I2C_ADDR    0x27  // Define I2C Address where the PCF8574A is
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
 
#define INVERT_SIGNAL true

#define BLINK_DELAY_MS 2400
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
SoftwareSerial mySerial =  SoftwareSerial(rxPin,txPin,INVERT_SIGNAL);
LiquidCrystal_I2C	lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

void init_board() {
 
 //call arduino init()
 init();
 mySerial.begin(BAUD_RATE);
#ifdef DEBUG
 Serial.begin(9600);
 DPRINTLN("Hello world");
 delay(2000);
#endif

  lcd.begin (20,4);

  // Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home ();                   // go home

  lcd.print("Webasto debug inter.");  
  lcd.setCursor ( 0, 1 );        // go to the 2nd line
  lcd.print("Just to test stuff!");
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
