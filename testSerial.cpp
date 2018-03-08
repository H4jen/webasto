#include <SoftwareSerial.h>
#include <avr/io.h>
#include <util/delay.h>
 
#define BLINK_DELAY_MS 1000
#define BAUD_RATE 300

#define setbit(port, bit) (port) |= (1 << (bit))
#define clearbit(port, bit) (port) &= ~(1 << (bit))
 
//SoftwareSerial mySerial(10, 11); // RX, TX

//can only be pins 8-13 because prtmapping is hardcoded to PORTB 
int8_t rxPin=10;
uint8_t txPin=13;

// set up a new serial port
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);
//byte pinState = 0;

void setup()  {
  // define pin modes for tx, rx, led pins:
  //pinMode(rxPin, INPUT);
  //pinMode(txPin, OUTPUT);
  //pinMode(ledPin, OUTPUT);
  // set the data rate for the SoftwareSerial port
  //mySerial.begin(BAUD_RATE);
  //mySerial.println("Hello World - SoftwareSerial");
}

void init_board_pins() {

 // set pin 5 of PORTB for output, diode blink
 //setbit(DDRB,5);
 
 //serial communications setup
 // set pin 11 as TX output
 //setbit(DDRB,txPin);

 //set pin 10 as RX input
 //clearbit(DDRB,rxPin);
 mySerial.begin(BAUD_RATE);

}


int main (void)
{

//Init all arduino pins
init_board_pins();

// main loop
while(1) {
  /* set pin 5 high to turn led on */
  //PORTB |= _BV(PORTB5);
  //_delay_ms(BLINK_DELAY_MS);

  /* set pin 5 low to turn led off */
  //PORTB &= ~_BV(PORTB5);
  _delay_ms(BLINK_DELAY_MS);
  mySerial.println("Hello World - SoftwareSerial");
 }
}
