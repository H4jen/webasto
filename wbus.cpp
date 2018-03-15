#include "wbus.h"
#include "webasto.h"

//ms to wait after a send to the UART
#define SEND_TX_DELAY 1000

const int TX_MESSAGE_INIT_1[]={6,0xF4,0x03,0x51,0x0A,0xAC};
const int TX_MESSAGE_INIT_2[]={6,0xF4,0x03,0x45,0x31,0x83};
const int TX_MESSAGE_INIT_3[]={6,0xF4,0x03,0x51,0x31,0x97};
const int TX_MESSAGE_INIT_4[]={6,0xF4,0x03,0x51,0x0c,0xAA};
const int TX_MESSAGE_INIT_5[]={5,0xF4,0x02,0x38,0xCE};
const int TX_MESSAGE_INIT_6[]={6,0xF4,0x03,0x53,0x02,0xA6};
const int TX_MESSAGE_INIT_7[]={6,0xF4,0x03,0x57,0x01,0xA1};

//Uses the global
void parse_message() {
  //do stuff
}


//struct rx_message rx_msg;

void w_bus::sendTXmessage(const int msg[])
{
    for(int i=1;i<msg[0];i++) Serial1.write(msg[i]);
    //delay(SEND_TX_DELAY);
    
}

    //WBUS constructor.
w_bus::w_bus () {
    //Open com with serial port to webasto heater.
    Serial1.begin(BAUDRATE,PARITY);
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
     sendTXmessage(TX_MESSAGE_INIT_1);
     //sendTXmessage(TX_MESSAGE_INIT_2);
     //sendTXmessage(TX_MESSAGE_INIT_3);
     //sendTXmessage(TX_MESSAGE_INIT_4);
     //sendTXmessage(TX_MESSAGE_INIT_5);
     //sendTXmessage(TX_MESSAGE_INIT_6);
     //sendTXmessage(TX_MESSAGE_INIT_7);
       
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
 //DPRINTLN("im here2");
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
        else rx_state = RESET_STATE;
        break;
    case READDATA:
            rxByte = Serial1.read();
                rx_msg.data[rx_msg.nr_data_read] = rxByte;
        rx_msg.nr_data_read++;
        if(rx_msg.nr_data_read >= rx_msg.length){
             int XOR = 0;
             XOR = XOR^rx_msg.header^rx_msg.length;
             for(int i=0;i<(rx_msg.length-1);i++) {
                XOR = XOR ^ rx_msg.data[i];
             }
             rx_msg.checksum = rxByte;
             if(XOR == rx_msg.checksum) {
                rx_state = PARSE_MESSAGE;
                rx_msg.valid_message = true;
                //Print debug message
                printMsgDebug();
                //calls message parser with global struct
                parse_message();
             }
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
        rx_msg.valid_message=false;
        rx_state=START;
        break;
    default:
        break;
    }
 }
}

