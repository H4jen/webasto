#include <Arduino.h>
#include <CustomSoftwareSerial.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>


#define I2C_ADDR    0x27  // Define I2C Address where the PCF8574A is
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7


#define MESSAGE_BUFFER_SIZE 128
//Change this is other header is needed. My air top evo 40 uses F4 from WTT side and 4F from multicontrol / heater side.
#define TXHEADER 0xf4
#define RXHEADER 0x4f
#define INVERT_SIGNAL false

#define BLINK_DELAY_MS 2400
//Baud rate of software serial (interface to Heater)
#define BAUD_RATE 2400

#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTHEX(...) debugprinthex(__VA_ARGS__,0)
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
  #define DPRINTLNHEX(...) debugprinthex(__VA_ARGS__,1)
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
  #define DPRINTHEX(...)
  #define DPRINTLNHEX(...)
#endif


//A test message
struct rx_message
{
  int header=0;
  int length=0;
  int data[MESSAGE_BUFFER_SIZE];
  int nr_data_read=0;
  int checksum=0; 
} rx_msg;

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
uint8_t rxPin=10;
uint8_t txPin=11;

// set up a new serial port
CustomSoftwareSerial* mySerial;

LiquidCrystal_I2C	lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

void init_board() {
 
 init();
 mySerial = new CustomSoftwareSerial(rxPin, txPin); // rx, tx
 mySerial->begin(BAUD_RATE, CSERIAL_8E1);         // Baud rate: 9600, configuration: CSERIAL_8N1

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


enum rx_reception_states {START, FINDHEADER, READLENGTH, READDATA, RESET_STATE, CHECKSUM_CHECK, PARSE_MESSAGE};
//Keeps track of which state the RX is in.,
enum rx_reception_states rx_state = START;

//Webasto communication. It is assumed that it is always the master (WTT) that initiates and requests information from the slave (heater)
//The read serial data function reads the serial data until a valid header is found. When the header is found it continues to read the message. 
//When a valid message has been read, the function waits to read a valid RX response. The TX com is always echoed back via K-line so this will work.
void readSerialData(void)
{
 //The reception of a message is implemented as a state machine 
 //Read is blocking. Wait until a message is read before doing anything else...
 //Need timeout?
 while (mySerial->available()){
 	//int txMsgLength=0;
	int rxByte = 0;
 	switch(rx_state) {
	//Nothing is received yet, but data in buffer
	case START:
		rx_state = FINDHEADER;
		break;	
	case FINDHEADER:
		rxByte = mySerial->read();
		//Below if statement could be flawed!!
		if((rxByte == TXHEADER) || (rxByte == RXHEADER)) {
		        rx_state = READLENGTH;
			rx_msg.header = rxByte;
		}
		break;
	//Header received
	case READLENGTH:
		rxByte = mySerial->read();
		if((rxByte < 128) && (rxByte > 1)) {
			rx_state =READDATA;
			rx_msg.length=rxByte;
		}
		else rx_state = RESET_STATE;
		break;
	case READDATA:
        	rxByte = mySerial->read();
                rx_msg.data[rx_msg.nr_data_read] = rxByte;
		rx_msg.nr_data_read++;
		if(rx_msg.nr_data_read >= rx_msg.length){
			 DPRINT("Header byte: ");
			 DPRINTLNHEX(rx_msg.header);
			 DPRINT("Length byte: ");
                         DPRINTLNHEX(rx_msg.length);
	    		 DPRINT("Data bytes: ");
			 int XOR = 0;
			 XOR = XOR^rx_msg.header^rx_msg.length;
			 for(unsigned int i=0;i<(rx_msg.length-1);i++) {
				XOR = XOR ^ rx_msg.data[i];
				DPRINTHEX(rx_msg.data[i]);
			 }
			 DPRINTLN(' ');
			 DPRINT("Checksum byte: ");
			 rx_msg.checksum = rxByte;
			 DPRINTLNHEX(rxByte);
			 
			 if(XOR == rx_msg.checksum) rx_state = PARSE_MESSAGE;
			 else {
			 	rx_state = RESET_STATE;
				DPRINTLN("Checksum check FAILED!!");
			 }
		}
                break;
	case PARSE_MESSAGE:
		//Do message parsing
		rx_state = RESET_STATE;
		break;
	case RESET_STATE:
		rx_msg.header=0;
  		rx_msg.length=0;
  		rx_msg.nr_data_read=0;
		rx_msg.checksum=0;
		rx_state=START;
		DPRINTLN(' '); 
		break;
 	default:
		break;
	}
 }
}


int main (void)
{
//Init arduino
init_board();

// main loop
while(1) {
  // Char from serial line interface
  //int c = mySerial->read();
  readSerialData();
  //if(rx_msg.size != 0){
//	for(int i=0; i<rx_msg.size;i++){
  //		DPRINTHEX(rx_msg.string[i]);
 // 	}
  //	DPRINTLN(' ');
  //}
  //If data is availible 
  //if(c != -1) { 
//	if(c == 0xF4){
//	  DPRINTLN(' ');
//	  DPRINTHEX(c);
//	}
//	else if(c <= 15){
//	  DPRINT('0');
  //        DPRINTHEX(c);
//	}
//	else { DPRINTHEX(c);
//	}
	
//	DPRINT(' ');
	

 }
}
