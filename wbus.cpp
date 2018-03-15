#include "wbus.h"
#include "webasto.h"

const int TX_MESSAGE_INIT_1[]={6,0xF4,0x03,0x51,0x0A,0xAC};

//Uses the global
void parse_message() {
  //do stuff
}


struct rx_message rx_msg;

void w_bus::sendTXmessage(const int msg[])
{
for(int i=1;i<msg[0];i++) mySerial->write(msg[i]);
}

//WBUS constructor.
w_bus::w_bus () {
    //Open com with serial port to webasto heater.
    mySerial = new CustomSoftwareSerial(rxPin, txPin); // rx, tx
    mySerial->begin(BAUDRATE, PARITY);         // Baud rate: 9600, configuration: CSERIAL_8N1
}


void w_bus::readSerialData(void)
{
 //The reception of a message is implemented as a state machine 
 //Read is blocking. Wait until a message is read before doing anything else...
 //Need timeout?
 while (mySerial->available()){
    int rxByte = 0;

    switch(rx_state) {
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

             if(XOR == rx_msg.checksum) {
                rx_state = PARSE_MESSAGE;
                rx_msg.valid_message = true;
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
        DPRINTLN(' '); 
        break;
    default:
        break;
    }
 }
}

