#include "message.h"
#include "webasto.h"

const int TX_MESSAGE_INIT_1[]={6,0xF4,0x03,0x51,0x0A,0xAC};

//Uses the global
void parse_message() {
   //do stuff
   
}

struct rx_message rx_msg;

void sendTXmessage(const int msg[])
{
for(int i=1;i<msg[0];i++) mySerial->write(msg[i]);
}

