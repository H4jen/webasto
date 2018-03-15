#ifndef MESSAGE_H
#define MESSAGE_H

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

//put your function headers here
void parse_message();

void sendTXmessage(const int msg[]);
#endif
