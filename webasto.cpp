#include <Arduino.h>
//#include <CustomSoftwareSerial.h>
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

LiquidCrystal_I2C	lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

void init_board() {
 
 init();

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

//Initialize the main wbus com object.
class w_bus wbus;


enum MAIN_STATES {START, INIT_WBUS, WBUS_OK};
enum MAIN_STATES MAIN_STATE = START;
    
// main loop
while(1) {

  //Detect if webasto communication is up and running. If not try to start (inf?)
  //Display status in LCD display.


    switch(MAIN_STATE) {

     case START:
        MAIN_STATE = INIT_WBUS;
        wbus.sendSerialBreak();
        break; 
     case INIT_WBUS:
        wbus.initSequence();
        if(wbus.wbus_ok == 1) MAIN_STATE = WBUS_OK;
        else if (wbus.wbus_ok == -1) MAIN_STATE = START;
        break;
     case WBUS_OK:
        wbus.initSequence();
        //if(wbus.wbus_ok) MAIN_STATE = WBUS_OK
        break;
    }
   
    delay(3000);
    //wbus.sendSerialBreak();
    //wbus.sendTXmessage(TX_MESSAGE_INIT_1);

    //Blocks until all serial data is read!
    while (Serial1.available()) {
        wbus.readSerialData();
    } 
 }
}
