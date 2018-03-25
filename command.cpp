#include "command.h"
//#include "webasto.h"

// Uses the global message captured.
// To reach this point the message needs to be RX and valid
// This is implicitly assumed.

    //WBUS constructor.
Command::Command () {
    
    write_array_counter=0;
    read_array_counter=0;

    //Let message_buffer pointer point to first index of buffer.
    write_ptr = &message_buffer[0];
    read_ptr = &message_buffer[0];
}

void Command::printMsgDebug(void)
{
    DPRINTLN(); 
}


void Command::readSerialData(void)
{

    int rxByte;
    rxByte = Serial.read();
    
    //DPRINTLN((char)rxByte);
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


//struct command_message
//{
//  char data_string[MESSAGE_BUFFER_SIZE];
//  int nr_data_read=0;
//  bool valid_message = false;
//};
void Command::getSerialMessage(void) {

    static int command_counter=0;
    static int read_array_counter = 0;
    bool reset_string_flag = false;

    //Check that there are new bytes to read. If write_ptr != read_ptr data is ready to read.
    if(write_ptr == read_ptr) return;

    cmd_msg.data_string[command_counter] = (char) *read_ptr;

    //    command_counter++;

    //Check for carrier return and parse.
    if((*read_ptr) == 13) {
        //replace '\n' with '\0' NULL
        //*read_ptr = 0;
        parseCommand(command_counter);
        reset_string_flag = true;
    }

    //Increase counter.
    command_counter++;
    
    if(command_counter > (MESSAGE_BUFFER_SIZE-1)) reset_string_flag = true;

    if(reset_string_flag) {
        command_counter =0;
        reset_string_flag = false;
    }

    read_array_counter++;
    read_ptr++;    
    // Reached end of buffer, wrap around.
    if(read_array_counter >= MESSAGE_BUFFER_SIZE) {
        read_array_counter = 0;
        read_ptr = &message_buffer[0];
    }


}

void Command::parseCommand(int string_size) {

    
    //If string empty return
    if(string_size < 1) return;
    
    //Parse data if not empty.
    if(strncmp(cmd_msg.data_string,"on",string_size) == 0) {
        DPRINTLN("WOW we are turning heater on");
    }
}
