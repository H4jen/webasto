#include "wbus.h"
#include "webasto.h"

#define RAWDEBUGOUTPUT

//ms to wait after a send to the UART
#define SEND_TX_DELAY 300
// Get Wbus version
const int TX_MESSAGE_INIT_1[]={6,0xF4,0x03,0x51,0x0A,0xAC};
//const int TX_MESSAGE_INIT_2[]={6,0xF4,0x03,0x45,0x31,0x83};
//Get units device name
const int TX_MESSAGE_INIT_2[]={6,0xF4,0x03,0x51,0x0b,0xAD};
//const int TX_MESSAGE_INIT_3[]={6,0xF4,0x03,0x51,0x31,0x97};
const int TX_MESSAGE_INIT_3[]={6,0xF4,0x03,0x51,0x0c,0xAA};
const int TX_MESSAGE_INIT_4[]={5,0xF4,0x02,0x38,0xCE};
const int TX_MESSAGE_INIT_5[]={6,0xF4,0x03,0x53,0x02,0xA6};
const int TX_MESSAGE_INIT_6[]={6,0xF4,0x03,0x57,0x01,0xA1};

//Status message sent in loop.
const int TX_STATUS_1[]={6,0xF4,0x03,0x56,0x01,0xA0};
const int TX_STATUS_2[]={34,0xF4,0x1F,0x50,0x30,0x01, 
                           0x03,0x05,0x06,0x07,0x08,
                           0x0A,0x0C,0x0E,0x0F,0x10,
                           0x11,0x13,0x1E,0x1F,0x24, 
                           0x27,0x29,0x2A,0x2C,0x2D,
                           0x32,0x34,0x3D,0x52,0x57,
                           0x5F,0x78,0x89};


String w_bus::subStringDataMsg(int index)
{
String str = rx_msg.data_string;
if(index >= rx_msg.length-1) return String("index to large");

str = str.substring(index,rx_msg.length-1);
return str;
}


// Uses the global message captured.
// To reach this point the message needs to be RX and valid
// This is implicitly assumed.
void w_bus::parseMessage() {

    //Main parser function
    switch(rx_msg.data[0]){
    //Message is a WBUS read message (type 51).
    case (0xd1):
        switch(rx_msg.data[1]) {
        //Version of Wbus
        case (0x0a):
            int nibble;
            nibble = ((rx_msg.data[2] >> 4) & 0x0f);
            DPRINT("WBUS version: "); DPRINT(nibble);DPRINT(".");
            nibble = ((rx_msg.data[2] & 0x0f));
            DPRINTLN(nibble);
            break;
        //Device name connected
        case (0x0b):
            DPRINT("Webasto heater control unit: "); 
            DPRINTLN(subStringDataMsg(2));
            break;
        //Device name connected
        case (0x0c):
            DPRINTLN("Supported sub-systems: (not implemented in code) "); 
            //DPRINTLN(subStringDataMsg(2));
            break;
        }
        break;
    //Ack from diagnosis software (type 38)
    case(0xb8):
        DPRINTLN("Diagnosis SW ack?"); 
        break;
    //Voltage diagnosis thresholds  (type 53)
    case(0xd3):
        DPRINTLN("Voltage diagnosis thresholds (not implemented in code)"); 
        break;
    //CO2 calibration (typ 57)
    case(0xd7):
        DPRINTLN("CO2 calibration (not implemented in code)"); 
        break;
    //Error codes(typ 56) 
    case(0xd6):
        DPRINTLN("Error codes status (not implemented in code)"); 
        break;
    default:
        printMsgDebug();
    }
}


//struct rx_message rx_msg;

void w_bus::sendTXmessage(const int msg[])
{
    //reset response counter
    number_of_rx_loops = 0;
    //TX sent flag
    waiting_for_rx_response = true;
    for(int i=1;i<msg[0];i++) Serial1.write(msg[i]);
    delay(SEND_TX_DELAY);
    
}

    //WBUS constructor.
w_bus::w_bus () {
    //Open com with serial port to webasto heater.
    Serial1.begin(BAUDRATE,PARITY);
    wbus_ok = false;
}


void w_bus::sendSerialBreak(void)
{
    //Turn off serial port
    Serial1.end();
    digitalWrite(txPin, HIGH);  // Send delay HIGH IDLE
    delay(1000);              // for 10ms
    digitalWrite(txPin, LOW);  // Set serial break
    delay(50); //wait default 50ms 
    digitalWrite(txPin, HIGH);  // reset serial break
    delay(50); //wait default 50ms
    Serial1.begin(BAUDRATE,PARITY);
}

void w_bus::initSequence(void) 
{
    static int counter = 0;
    if(waiting_for_rx_response == true) return;
    if(counter == 0) {
        sendTXmessage(TX_MESSAGE_INIT_1);
    }
    if (counter == 2){
        sendTXmessage(TX_MESSAGE_INIT_2);
    }
    if (counter == 3){
        sendTXmessage(TX_MESSAGE_INIT_3);
    } 
    if (counter == 4){
        sendTXmessage(TX_MESSAGE_INIT_4);
    }
    if (counter == 5){
        sendTXmessage(TX_MESSAGE_INIT_5);
    }
    if (counter == 6){
        sendTXmessage(TX_MESSAGE_INIT_6);
    }
    if (counter == 7){
        sendTXmessage(TX_STATUS_1);
    }
    if (counter == 8){
        counter = 0;
        wbus_ok = true;
        sendTXmessage(TX_STATUS_2);
        return;
    }
    
   counter++; 
}

void w_bus::printMsgDebug(void)
{
    DPRINTHEX(rx_msg.header);
    DPRINT(' ');
    DPRINTHEX(rx_msg.length);
    DPRINT(' ');
    for(int i=0;i<(rx_msg.length-1);i++) {
        DPRINTHEX(rx_msg.data[i]);
        DPRINT(' ');
    }
    DPRINTHEX(rx_msg.checksum);
    DPRINTLN(); 
}


void w_bus::readSerialData(void)
{
 //The reception of a message is implemented as a state machine 
 //Read is blocking. Wait until a message is read before doing anything else...
 //Need timeout? 
 
 //Do some checking if we are waiting for response and if we have had som timeouts
 //
 
 if(waiting_for_rx_response) {
   number_of_rx_loops++;
   if (number_of_rx_loops > 30){ 
       waiting_for_rx_response = false;
       DPRINTLN("Waiting for RX response timed!"); 
       time_out_loops++;
   }
   if(time_out_loops > 10) {
       DPRINTLN("Communication down on W-bus.. NO response");
       //Signal that wbus needs to be restarted
       wbus_ok = false;
   }
 }
  
  while (Serial1.available()>0){
    int rxByte = 0;
    switch(rx_state) {
    case START:
        rx_state = FINDHEADER;
        break;  
    case FINDHEADER:
        rxByte = Serial1.read();
        //Below if statement could be flawed!!
        if((rxByte == TXHEADER) || (rxByte == RXHEADER)) {
                rx_state = READLENGTH;
                rx_msg.header = rxByte;
        }
        break;
    //Header received
    case READLENGTH:
        rxByte = Serial1.read();
        if((rxByte < 128) && (rxByte > 1)) {
            rx_state =READDATA;
            rx_msg.length=rxByte;
        }
        else {
            rx_state = RESET_STATE;
        }
        break;
    case READDATA:
        rxByte = Serial1.read();
        rx_msg.data[rx_msg.nr_data_read] = rxByte;
        rx_msg.data_string[rx_msg.nr_data_read] = (char)rxByte;
        rx_msg.nr_data_read++;
        if(rx_msg.nr_data_read >= rx_msg.length){
             int XOR = 0;
             XOR = XOR^rx_msg.header^rx_msg.length;
             //terminate data_string read
             rx_msg.data_string[rx_msg.nr_data_read]='\0';
             for(int i=0;i<(rx_msg.length-1);i++) {
                XOR = XOR ^ rx_msg.data[i];
             }
             rx_msg.checksum = rxByte;
             if(XOR == rx_msg.checksum) {
                rx_state = RESET_STATE;
                rx_msg.valid_message = true;
                //Print debug message
                #ifdef RAWDEBUGOUTPUT
                    printMsgDebug();
                #endif
                //Do parsing and shit if we received an RX message
                if(rx_msg.header == RXHEADER) {
                    //calls message parser with global struct
                    parseMessage();
                    //Clear TX response flag
                    waiting_for_rx_response = false;
                    //Clear timeout loops
                    time_out_loops=0;
                }
             }
             else {
                rx_state = RESET_STATE;
                DPRINTLN("Checksum check FAILED!!");
             }
        }
                break;
    case RESET_STATE:
        rx_msg.header=0;
        rx_msg.length=0;
        rx_msg.nr_data_read=0;
        rx_msg.checksum=0;
        rx_msg.valid_message=false;
        rx_state=START;
        break;
    default:
        break;
    }
 }
}

