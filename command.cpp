#include "command.h"
//#include "webasto.h"

// Uses the global message captured.
// To reach this point the message needs to be RX and valid
// This is implicitly assumed.

    //WBUS constructor.
Command::Command () {
    //Open com with serial port to webasto heater.
    //Serial1.begin(BAUDRATE,PARITY);
    //wbus_ok = false;
    
    //Let message_buffer pointer point to first index of buffer.
    write_ptr = &message_buffer[0];
    read_ptr = &message_buffer[0];
}

void Command::printMsgDebug(void)
{
    DPRINTHEX(cmd_msg.header);
    //DPRINT(' ');
    DPRINTHEX(cmd_msg.length);
    //DPRINT(' ');
    for(int i=0;i<(cmd_msg.length-1);i++) {
        DPRINTHEX(cmd_msg.data[i]);
        //DPRINT(' ');
    }
    //DPRINTHEX(cmd_msg.checksum);
    DPRINTLN(); 
}


void Command::readSerialData(void)
{

    int rxByte = Serial1.read();
    
    //write read value to current index
    *write_ptr = rxByte;
    write_array_counter++;
    write_ptr++;
    // Reached end of buffer, wrap around.
    if(write_array_counter >= MESSAGE_BUFFER_SIZE) {
        write_array_counter = 0;
        write_ptr = &message_buffer[0];
    }
        
}

void Command::getSerialMessage(void) {

//Check that there are new bytes to read. If write_ptr != read_ptr data is ready to read.
if(write_ptr == read_ptr) return;
int rxByte = 0;

/*    
//Read data ONLY if we are in correct state...
if(!((rx_state == START) || (rx_state == RESET_STATE))) {
    rxByte = *read_ptr;
    //DPRINTLNHEX(rxByte);
    read_array_counter++;
    read_ptr++;

    // Reached end of buffer, wrap around.
    if(read_array_counter >= MESSAGE_BUFFER_SIZE) {
        read_array_counter = 0;
        read_ptr = &message_buffer[0];
    }*/
}

//Find message in data
/* 
 switch(rx_state) {
    case START:
        rx_state = FINDHEADER;
        break;  
    case FINDHEADER:
        //Below if statement could be flawed!!
        if((rxByte == TXHEADER) || (rxByte == RXHEADER)) {
                rx_state = READLENGTH;
                rx_msg.header = rxByte;
                //DPRINTLNHEX(rxByte);
        }
        break;
    //Header received
    case READLENGTH:
        //DPRINTLNHEX(rxByte);
        if((rxByte < 256) && (rxByte > 1)) {
            rx_state =READDATA;
            rx_msg.length=rxByte;
        }
        else {
            rx_state = RESET_STATE;
        }
        break;
    case READDATA:
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
                //if(rx_msg.header == RXHEADER) {
                    //calls message parser with global struct
                    //parseMessage();
                    //Clear TX response flag
                    //DPRINTLN("GOOD msg!!");
                    //waiting_for_rx_response = false;
                    //Clear timeout loops
                //}
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
}*/
