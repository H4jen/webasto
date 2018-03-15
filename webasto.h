#ifndef WEBASTO_H
#define WEBASTO_H

#define MESSAGE_BUFFER_SIZE 128

#include <CustomSoftwareSerial.h>

//Here we define global variables we need to reference
//This global struct holds the message read from the webasto.
extern CustomSoftwareSerial* mySerial;

#endif
