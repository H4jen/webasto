#ifndef WBUS_H
#define WBUS_H

#include <Arduino.h>
#include "webasto.h"

//extern const int TX_MESSAGE_INIT_1[];
//extern const int TX_MESSAGE_INIT_2[];
//extern const int TX_MESSAGE_INIT_3[];
//extern const int TX_MESSAGE_INIT_4[];
//extern const int TX_MESSAGE_INIT_5[];
//extern const int TX_MESSAGE_INIT_6[];


//This struct contains all the status information from webasto
struct webasto_status 
{
    int status_01 = 0;
    int status_03 = 0;
    int status_05 = 0;
    int status_06 = 0;
    int status_07 = 0;
    int status_08 = 0;
    int status_0A = 0;
    int temp = 0;
    int status_0F = 0;

    int status_10 = 0;
    int power = 0;
    int resistance = 0;
    int comb_fan = 0;
    int status_1F = 0;
    int status_24 = 0;
    int status_27 = 0;
    int status_29 = 0;
    int status_2A = 0;
    int status_2C = 0;
    int status_2D = 0;
    int status_32 = 0;    
    int status_34 = 0;
    
    //Status information
    int voltage_mV = 0;
    
    //On-off flags (msg 50:03)
    
/*
    bool combustion_fan=false;
    bool glow_plug = false;
    bool fuel_pump = false;
    bool circulation_pump = false;
    bool vehicle_fan_relay = false;
    bool noozle_stock_heating = false;
    bool flame_indicator = false;
    
    //Operational measurements msg 50:05
    int temp=-99;
    int milliVolt=0;
    bool flameDet=false;
    int heatPower=0;
    int flameRes=0;
  */  
};



struct rx_message
{
  int rawMessage=0;
  int header=0;
  int length=0;
  int data[MESSAGE_BUFFER_SIZE];
  char data_string[MESSAGE_BUFFER_SIZE];
  int nr_data_read=0;
  int checksum=0; 
  bool valid_message = false;
};

//Define the wbus class
class w_bus {
    //Holds communication object
    //CustomSoftwareSerial* mySerial;
    //can only be pins 8-13 because prtmapping is hardcoded to PORTB 
    
    //Wraparound message buffer;
    int message_buffer[MESSAGE_BUFFER_SIZE];
    //Tracks were read and write in buffer is. Strategy is to always write to buffer when data availible. Read of buffer is only done when data is avalible.
    int *read_ptr,*write_ptr;
    int write_array_counter = 0;
    int read_array_counter = 0;

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
    //The flag below is implemented so that a sent request should result in a RX response.
    bool waiting_for_rx_response = false;
    //Number of external loops before timeout and rx_response cleared.
    int  time_out_loops=10;
    //Keeps track of the number of times trying to read from RX without reponse. Reset, when a succespul read has been done.
    int  number_of_rx_loops=0;
    //int wbus_ok=0;
    String subStringDataMsg(int index);
    //This function is called if a type 50 message is received (status)
    void parseStatusData();
    //struct webasto_status wbus_status;
    
   
public:
    bool wbus_ok=false;
    void parseMessage(void);
    w_bus();
    void printMsgDebug(void);
    void sendSerialBreak(void);
    void sendTXmessage(const int msg[],bool need_ack);
    
    void readSerialData(void);
    void getSerialMessage(void);
    void initSequence(void);
    void statusSequence(void);
    
    struct webasto_status wbus_status;
};


//put your function headers here
//void parse_message();
//void sendTXmessage(const int msg[]);
#endif
