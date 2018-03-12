#include <Arduino.h>
//#include <HardwareSerial.h>
#include <CustomSoftwareSerial.h>
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
 
#define INVERT_SIGNAL false

#define BLINK_DELAY_MS 2400
//Baud rate of software serial (interface to Heater)
#define BAUD_RATE 2400
#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.

#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTHEX(...) Serial.print(__VA_ARGS__,HEX)
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
  #define DPRINTLNHEX(...) Serial.println(__VA_ARGS__,HEX);
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
  #define DPRINTHEX(...)
  #define DPRINTLNHEX(...)
#endif

//byte message[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//byte message2[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

//#define setbit(port, bit) (port) |= (1 << (bit))
//#define clearbit(port, bit) (port) &= ~(1 << (bit))
 
//can only be pins 8-13 because prtmapping is hardcoded to PORTB 
uint8_t rxPin=10;
uint8_t txPin=11;

// set up a new serial port
CustomSoftwareSerial* mySerial;
//=  SoftwareSerial(rxPin,txPin,INVERT_SIGNAL);

LiquidCrystal_I2C	lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

void init_board() {
 
 //call arduino init()
 init();
 mySerial = new CustomSoftwareSerial(rxPin, txPin); // rx, tx
 mySerial->begin(2400, CSERIAL_8E1);         // Baud rate: 9600, configuration: CSERIAL_8N1
 //mySerial->write("Test message");            // Write testing data

//mySerial.begin(BAUD_RATE);


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
//int FFcounter = 0;
// main loop
while(1) {
  int c = mySerial->read();
  if(c != -1) { 
	if(c == 0xF4){
	  DPRINTLN(' ');
	  DPRINTHEX(c);
	}
	else { DPRINTHEX(c);}
	DPRINT(' ');
	
  }
 }
}
