#ifndef WBUS_H
#define WBUS_H

#include <CustomSoftwareSerial.h>
#include "webasto.h"

extern const int TX_MESSAGE_INIT_1[];

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
    CustomSoftwareSerial* mySerial;
    //can only be pins 8-13 because prtmapping is hardcoded to PORTB 
    uint8_t rxPin=10;
    uint8_t txPin=11;
    int  BAUDRATE = 2400;
    int  PARITY = CSERIAL_8E1;
    int  TXHEADER = 0xf4;
    int RXHEADER = 0x4f;
    enum rx_reception_states {START, FINDHEADER, READLENGTH, READDATA, RESET_STATE, CHECKSUM_CHECK, PARSE_MESSAGE};
    enum rx_reception_states rx_state = START;

public:
    w_bus();
    void sendTXmessage(const int msg[]);
    void readSerialData(void);

};


//put your function headers here
//void parse_message();
//void sendTXmessage(const int msg[]);
#endif
