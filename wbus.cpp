#include "wbus.h"
#include "webasto.h"

//ms to wait after a send to the UART
#define SEND_TX_DELAY 300
const int TX_MESSAGE_INIT_1[]={6,0xF4,0x03,0x51,0x0A,0xAC};
const int TX_MESSAGE_INIT_2[]={6,0xF4,0x03,0x45,0x31,0x83};
const int TX_MESSAGE_INIT_3[]={6,0xF4,0x03,0x51,0x31,0x97};
const int TX_MESSAGE_INIT_4[]={6,0xF4,0x03,0x51,0x0c,0xAA};
const int TX_MESSAGE_INIT_5[]={5,0xF4,0x02,0x38,0xCE};
const int TX_MESSAGE_INIT_6[]={6,0xF4,0x03,0x53,0x02,0xA6};
const int TX_MESSAGE_INIT_7[]={6,0xF4,0x03,0x57,0x01,0xA1};

//Status message sent in loop.
const int TX_STATUS_1[]={6,0xF4,0x03,0x56,0x01,0xA0};


//Uses the global
void parse_message() {
  //do stuff
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
        sendTXmessage(TX_MESSAGE_INIT_7);
    }
    //If we reach below this we have finished init
    if (counter == 8){
        counter = 0;
        wbus_ok = true;
        sendTXmessage(TX_STATUS_1);
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
                //Do parsing and shit if we received an RX message
                if(rx_msg.header == RXHEADER) {
                    //calls message parser with global struct
                    parse_message();
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

