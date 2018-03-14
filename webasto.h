#ifndef WEBASTO_H
#define WEBASTO_H

#define MESSAGE_BUFFER_SIZE 128

//Here we define global variables we need to reference
//This global struct holds the message read from the webasto.
struct rx_message
{
  int header=0;
  int length=0;
  int data[MESSAGE_BUFFER_SIZE];
  int nr_data_read=0;
  int checksum=0; 
  bool valid_message = false;
} rx_msg;

#endif
