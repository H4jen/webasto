#ifndef WBUS_H
#define WBUS_H

#include <Arduino.h>
//#include <CustomSoftwareSerial.h>
#include "webasto.h"

extern const int TX_MESSAGE_INIT_1[];
extern const int TX_MESSAGE_INIT_2[];
extern const int TX_MESSAGE_INIT_3[];
extern const int TX_MESSAGE_INIT_4[];
extern const int TX_MESSAGE_INIT_5[];
extern const int TX_MESSAGE_INIT_6[];
//extern const int TX_MESSAGE_INIT_1[];

struct rx_message
{
  int header=0;
  int length=0;
  int data[MESSAGE_BUFFER_SIZE];
  int nr_data_read=0;
  int checksum=0; 
  bool valid_message = false;
};

//Define the wbus class
class w_bus {
    //Holds communication object
    //CustomSoftwareSerial* mySerial;
    //can only be pins 8-13 because prtmapping is hardcoded to PORTB 
    struct rx_message rx_msg;
    uint8_t rxPin=19;
    uint8_t txPin=18;
    int  BAUDRATE = 2400;
    int  PARITY = SERIAL_8E1;
    int  TXHEADER = 0xf4;
    int  RXHEADER = 0x4f;
    enum rx_reception_states {START, FINDHEADER, READLENGTH, READDATA, RESET_STATE, CHECKSUM_CHECK, PARSE_MESSAGE};
    enum rx_reception_states rx_state = START;
    //indicates if the wbus is initialized and ready to go. (1 = ok, 0 = in progressm, -1 failed)
    //int wbus_ok=0;

public:
    int wbus_ok=0;
    
    w_bus();
    void printMsgDebug(void);
    void sendSerialBreak(void);
    void sendTXmessage(const int msg[]);
    void readSerialData(void);
    void initSequence(void);
};


//put your function headers here
//void parse_message();
//void sendTXmessage(const int msg[]);
#endif
