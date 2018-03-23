#ifndef COMMAND_H
#define COMMAND_H

#include <Arduino.h>
#include "webasto.h"

struct command_message
{
  char data_string[MESSAGE_BUFFER_SIZE];
};

//Define the wbus class
class Command {
    //Holds communication object
    //CustomSoftwareSerial* mySerial;
    //can only be pins 8-13 because prtmapping is hardcoded to PORTB 
    
    //Wraparound message buffer;
    int message_buffer[MESSAGE_BUFFER_SIZE];
    //Tracks were read and write in buffer is. Strategy is to always write to buffer when data availible. Read of buffer is only done when data is avalible.
    int *read_ptr,*write_ptr;
    int write_array_counter = 0;
    int read_array_counter = 0;

    struct command_message cmd_msg;
    uint8_t rxPin=19;
    uint8_t txPin=18;
    int  BAUDRATE = 2400;
    int  PARITY = SERIAL_8E1;
    int  TXHEADER = 0xf4;
    int  RXHEADER = 0x4f;
    //enum rx_reception_states {START, FINDHEADER, READLENGTH, READDATA, RESET_STATE, CHECKSUM_CHECK, PARSE_MESSAGE};
    //enum rx_reception_states rx_state = START;
    //indicates if the wbus is initialized and ready to go. (1 = ok, 0 = in progressm, -1 failed)
    //The flag below is implemented so that a sent request should result in a RX response.
    //bool waiting_for_rx_response = false;
    //Number of external loops before timeout and rx_response cleared.
    int  time_out_loops=10;
    //Keeps track of the number of times trying to read from RX without reponse. Reset, when a succespul read has been done.
    int  number_of_rx_loops=0;
    //int wbus_ok=0;
    String subStringDataMsg(int index);
    //This function is called if a type 50 message is received (status)
    //void parseStatusData(int pos);
    //struct webasto_status wbus_status;
    
   
public:
    //bool wbus_ok=false;
    void parseCommand(int string_size);
    Command();
    void printMsgDebug(void);
    //void sendSerialBreak(void);
    //void sendTXmessage(const int msg[],bool need_ack);
    
    //void getCommand()
    void readSerialData(void);
    void getSerialMessage(void);
    //void initSequence(void);
    //void statusSequence(void);
    
    //struct webasto_status wbus_status;
};


//put your function headers here
//void parse_message();
//void sendTXmessage(const int msg[]);
#endif
