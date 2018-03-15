#include <Arduino.h>
#include <CustomSoftwareSerial.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "wbus.h"
#include "webasto.h"

#define I2C_ADDR    0x27  // Define I2C Address where the PCF8574A is
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7


//Change this is other header is needed. My air top evo 40 uses F4 from WTT side and 4F from multicontrol / heater side.
//#define TXHEADER 0xf4
//#define RXHEADER 0x4f
#define INVERT_SIGNAL false

#define BLINK_DELAY_MS 2400

void debugprinthex(int c,int newline) 
{
  DPRINT(' ');
  if(c <= 15){
  	DPRINT('0');
        Serial.print(c,HEX);
  }
  else Serial.print(c,HEX);
  if(newline == 1) Serial.println();
}

//can only be pins 8-13 because prtmapping is hardcoded to PORTB 
//uint8_t rxPin=10;
//uint8_t txPin=11;

// set up a new serial port
//CustomSoftwareSerial* mySerial;
LiquidCrystal_I2C	lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

void init_board() {
 
 init();
 //mySerial = new CustomSoftwareSerial(rxPin, txPin); // rx, tx
 //mySerial->begin(BAUD_RATE, CSERIAL_8E1);         // Baud rate: 9600, configuration: CSERIAL_8N1

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

  lcd.print("Webasto");  
  lcd.setCursor ( 0, 1 );        // go to the 2nd line
  lcd.print("test stuff!");
}

int main (void)
{
//Init arduino. do this before anything else happens...
init_board();

//main comm object.
class w_bus wbus;

// main loop
while(1) {
  
  //Detect if webasto communication is up and running. If not try to start (inf?)
  //Display status in LCD display.
  wbus.sendSerialBreak();
  wbus.sendTXmessage(TX_MESSAGE_INIT_1);

 }
}
