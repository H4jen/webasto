#include <Arduino.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "wbus.h"
#include "webasto.h"
#include "command.h"

//The number of ms to wait until running status loop again
#define WEBASTO_LOOP_TIME 1000 

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
unsigned long current_tick=0;
unsigned long loop_tick=0;

//Init arduino. do this before anything else happens...
init_board();

//Initialize the main wbus com object, and serial command interface
class w_bus wbus;
class Command cmd;


enum MAIN_STATES {START, INIT_WBUS, WBUS_OK};
enum MAIN_STATES MAIN_STATE = START;
    
// main loop
while(1) {

    //Get time from start
    current_tick = millis();
    
    //TX part of code.
    switch(MAIN_STATE) {
     case START:
        MAIN_STATE = INIT_WBUS;
        wbus.sendSerialBreak();
        break; 
     case INIT_WBUS:
        if(current_tick > (loop_tick + WEBASTO_LOOP_TIME)){
            wbus.initSequence();
            loop_tick=current_tick;
        }
        if(wbus.wbus_ok) MAIN_STATE = WBUS_OK;
        break;
     case WBUS_OK:
        //Run this loop every WEBASTO_LOOP_TIME
        if(current_tick > (loop_tick + WEBASTO_LOOP_TIME)){
            //DPRINTLN(current_tick);
            wbus.statusSequence();
            loop_tick=current_tick;
        }
        if (!wbus.wbus_ok) {
            DPRINTLN("Trying to reset the Wbus");
            MAIN_STATE = START;
        }
        break;
    }
    
    // RX part of code.    
    //This is calles each loop, but only one byte is parsed each round
    wbus.getSerialMessage();
   
    //Always data all availible data from webasto if availible and push to wraparound buffer.
    while(Serial1.available()>0) {
        wbus.readSerialData();
    }

    
    //Get command set
    cmd.getSerialMessage();


    //Read one char from command interface if availible
    if (Serial.available()>0) {
        cmd.readSerialData();
    }
 
 }
}
