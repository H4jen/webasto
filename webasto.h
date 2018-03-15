#define BAUD_RATE 2400

#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG    //Macros are usually in all capital letters.
#define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
#define DPRINTHEX(...) debugprinthex(__VA_ARGS__,0)
#define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#define DPRINTLNHEX(...) debugprinthex(__VA_ARGS__,1)
#else
#define DPRINT(...)     //now defines a blank line
#define DPRINTLN(...)   //now defines a blank line
#define DPRINTHEX(...)
#define DPRINTLNHEX(...)
#endif
#ifndef WEBASTO_H
#define WEBASTO_H

#define MESSAGE_BUFFER_SIZE 128


#include <Arduino.h>

//Here we define global variables we need to reference
//This global struct holds the message read from the webasto.
//extern CustomSoftwareSerial* mySerial;

void debugprinthex(int c,int newline);


#endif
