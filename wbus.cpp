#include "wbus.h"
#include "webasto.h"

#define RAWDEBUGOUTPUT

//ms to wait after a send to the UART
#define SEND_TX_DELAY 300
// Get Wbus version
const int TX_MESSAGE_INIT_1[]={6,0xF4,0x03,0x51,0x0A,0xAC};
const int TX_MESSAGE_INIT_2[]={6,0xF4,0x03,0x45,0x31,0x83};
//Get units device name
const int TX_MESSAGE_INIT_3[]={6,0xF4,0x03,0x51,0x31,0x97};
const int TX_MESSAGE_INIT_4[]={25,0xF4,0x16,0x51,0x30,0x01,0x02,0x03,0x04,0x06,0x07,0x08,0x09,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x12,0x15,0x18,0x19,0x1A,0x80}; 
//                                            51   30   01   02   03   04   06   07   08 09 0B       0C 0D 0E       0F 10 12 15 18 19 1A
                             //1  2    3    4    5    6    7     8   9    10    11   2    3    4    5    6    7    8    9   20   1    2    3    4    5    6    7    8     9   30   1     2  3    4
const int TX_MESSAGE_INIT_5[]={6,0xF4,0x03,0x50,0x31,0x96};
const int TX_MESSAGE_INIT_6[]={6,0xF4,0x03,0x51,0x0c,0xaa};
const int TX_MESSAGE_INIT_7[]={5,0xF4,0x02,0x38,0xCE};
const int TX_MESSAGE_INIT_8[]={6,0xF4,0x03,0x53,0x02,0xA6};
const int TX_MESSAGE_INIT_9[]={6,0xF4,0x03,0x57,0x01,0xA1};

//Status message sent in loop.
const int TX_STATUS_1[]={6,0xF4,0x03,0x56,0x01,0xA0};
const int TX_STATUS_2[]={6,0xF4,0x03,0x50,0x03,0xA4};
const int TX_STATUS_3[]={6,0xF4,0x03,0x50,0x05,0xA2};
const int TX_STATUS_4[]={34,0xF4,0x1F,0x50,0x30,0x01,0x03,0x05,0x06,0x07,0x08,0x0A,0x0C,0x0E,0x0F,0x10,0x11,0x13,0x1E,0x1F,0x24,0x27,0x29,0x2A,0x2C,0x2D,0x32,0x34,0x3D,0x52,0x57,0x5F,0x78,0x89};
                       //1    2    3    4    5    6    7     8   9    10    11   2    3    4    5    6    7    8    9   20   1    2    3    4    5    6    7    8     9   30   1     2  3    4
//Keep alive message.
const int TX_KEEP_ALIVE[]={7,0xF4,0x04,0x44,0x2A,0x00,0x9E};


bool isBitSet(uint8_t num, int bit)
{
    return 1 == ( (num >> bit) & 1);
}

String w_bus::subStringDataMsg(int index)
{
String str = rx_msg.data_string;
if(index >= rx_msg.length-1) return String("index to large");

str = str.substring(index,rx_msg.length-1);
return str;
}


// Uses the global message captured.
// To reach this point the message needs to be RX and valid
// This is implicitly assumed.

void w_bus::parseMessage() {

    //Main parser function
    switch(rx_msg.data[0]){
    //Message is a WBUS read message (type 51).
    case (0xd1):
        switch(rx_msg.data[1]) {
        //Version of Wbus
        case (0x0a):
            int nibble;
            nibble = ((rx_msg.data[2] >> 4) & 0x0f);
            DPRINT("WBUS version: "); DPRINT(nibble);DPRINT(".");
            nibble = ((rx_msg.data[2] & 0x0f));
            DPRINTLN(nibble);
            break;
        //Device name connected
        case (0x0b):
            DPRINT("Webasto heater control unit: "); 
            DPRINTLN(subStringDataMsg(2));
            break;
        //Device name connected
        case (0x0c):
            DPRINTLN("Supported sub-systems: (not implemented in code) "); 
            //DPRINTLN(subStringDataMsg(2));
            break;
        }
        break;
    //Ack from diagnosis software (type 38)
    case(0xb8):
        DPRINTLN("Diagnosis SW ack?"); 
        break;
    //Voltage diagnosis thresholds  (type 53)
    case(0xd3):
        DPRINTLN("Voltage diagnosis thresholds (not implemented in code)"); 
        break;
    //CO2 calibration (typ 57)
    case(0xd7):
        DPRINTLN("CO2 calibration (not implemented in code)"); 
        break;
    //Error codes(typ 56) 
    case(0xd6):
        DPRINTLN("Error codes status (not implemented in code)"); 
        break;
    case(0xd0):
        switch(rx_msg.data[1]) {
            //Multi read
            case (0x30):
                //parseStatusData(2);
                //printMsgDebug();
                break;
            //Only one sensor
            default:
                parseStatusData(1);    
        }
        break;
    default:
        printMsgDebug();
    }
}

//Run through status data and parse each status output
// Normal status ouput below
// 4F 52 D0 30 01 00 03 00 05 00 06 00 07 00 08 00 
// 0A 00 0C 3C 0E 31 56 0F 00 75 10 00 11 00 00 
// 13 00 D8 1E 00 00 1F 00 24 00 27 32 29 8F A0 
// 2A 00 2C 00 2D 00 32 00 34 03 70 3D 00 00 52 00 00 30 00 03 05 00 0D 29 00 02 25 
// 57 00 19 0B 5F 00 22 78 00 75 17
// This functions assumes that the data status fields are fixed

void w_bus::parseStatusData(int pos) {

    //Keeps track of next status position
    int spos = pos;
    
    //Run through the message and sort out the different status informations.
    while(spos < rx_msg.length-1)
    {
      switch(rx_msg.data[spos]) {
    
        //Unknown
        case(0x01):
            wbus_status.status_01 = rx_msg.data[spos+1];
            spos = spos+2;
            break;
        
        //On-off flag of different systems (important)
        //
        //combustion_fan
        //bool glow_plug = false;
        //fuel_pump = false;
        //circulation_pump = false;
        //vehicle_fan_relay = false;
        //noozle_stock_heating = false;
        //flame_inidicator = false;
        case(0x03):
            int on_off_status; 
            on_off_status=rx_msg.data[spos+1];
            wbus_status.combustion_fan=isBitSet(on_off_status,0);
            wbus_status.glow_plug=isBitSet(on_off_status,1);
            wbus_status.fuel_pump=isBitSet(on_off_status,2);
            wbus_status.circulation_pump=isBitSet(on_off_status,3);
            wbus_status.vehicle_fan_relay=isBitSet(on_off_status,4);
            wbus_status.noozle_stock_heating=isBitSet(on_off_status,5);
            wbus_status.flame_indicator=isBitSet(on_off_status,6);
            DPRINT("Combustion fan = "); DPRINT(wbus_status.combustion_fan); DPRINT(' ');
            DPRINT("Glow plug = "); DPRINT(wbus_status.glow_plug); DPRINT(' ');
            DPRINT("Fuel pump = "); DPRINT(wbus_status.fuel_pump); DPRINT(' ');
            DPRINT("Circulation pump = "); DPRINT(wbus_status.circulation_pump); DPRINTLN();
            DPRINT("Vehicle_fan_relay = "); DPRINT(wbus_status.vehicle_fan_relay); DPRINT(' ');
            DPRINT("Noozle_stock_heating = "); DPRINT(wbus_status.noozle_stock_heating); DPRINT(' ');
            DPRINT("Flame indicator = "); DPRINT(wbus_status.flame_indicator); DPRINTLN(' ');            
            spos = spos+2;
            break;
        
        //Operational measurements
        // 4F 0B D0 05 3C 31 38 00 00 00 00 00 A4
        case(0x05):
            wbus_status.temp = rx_msg.data[spos+1]-50;
            //Big endian
            wbus_status.milliVolt = 256*rx_msg.data[spos+2]+rx_msg.data[spos+3];
            wbus_status.flameDet = isBitSet(rx_msg.data[spos+4],0);
            //Big endian
            wbus_status.heatPower = 256*rx_msg.data[spos+5]+rx_msg.data[spos+6];
            wbus_status.flameRes = 256*rx_msg.data[spos+7]+rx_msg.data[spos+8];

            DPRINT("temp = "); DPRINT(wbus_status.temp); DPRINTLN();
            DPRINT("Voltage (mV) = "); DPRINT(wbus_status.milliVolt); DPRINTLN();
            DPRINT("Flame detected ? = "); DPRINT(wbus_status.flameDet); DPRINTLN();
            DPRINT("Heater Power = "); DPRINT(wbus_status.heatPower); DPRINTLN();
            DPRINT("Flame detector resistanse (mOhm) = "); DPRINT(wbus_status.flameRes); DPRINTLN();
            spos = spos+9;
            break;
        
        default:
            //Jump out if unknown status command found
            DPRINTLNHEX(rx_msg.data[spos]);
            DPRINTLN("Unknown status response!!");
            spos = 999;
     }
    }
} 

//struct rx_message rx_msg;

void w_bus::sendTXmessage(const int msg[],bool need_ack)
{
    //DPRINTLN("Sending MTFO");
    //reset response counter
    number_of_rx_loops = 0;
    //TX sent flag
    //if(need_ack) waiting_for_rx_response = true;
    //DPRINT("Sending: ");
    for(int i=1;i<msg[0];i++) {
        Serial1.write(msg[i]);
    //    DPRINTHEX(msg[i]);
    }
    //DPRINTLN();
    //delay(SEND_TX_DELAY);
    
}

    //WBUS constructor.
w_bus::w_bus () {
    //Open com with serial port to webasto heater.
    Serial1.begin(BAUDRATE,PARITY);
    wbus_ok = false;
    
    //Let message_buffer pointer point to first index of buffer.
    write_ptr = &message_buffer[0];
    read_ptr = &message_buffer[0];
}


void w_bus::sendSerialBreak(void)
{
    //Turn off serial port
    Serial1.end();
    digitalWrite(txPin, HIGH);  // Send delay HIGH IDLE
    delay(1000);              // for 10ms
    digitalWrite(txPin, LOW);  // Set serial break
    delay(50); //wait default 50ms 
    digitalWrite(txPin, HIGH);  // reset serial break
    delay(50); //wait default 50ms
    Serial1.begin(BAUDRATE,PARITY);
}

void w_bus::statusSequence(void) {
    static int counter = 0;
    //if(waiting_for_rx_response == true) {DPRINTLN("waintn for RX!");return;}
    if(counter == 0) {
          sendTXmessage(TX_STATUS_1,true);
    }
    if(counter == 1){
          sendTXmessage(TX_STATUS_2,true);
    }
    if (counter == 2){
          sendTXmessage(TX_STATUS_3,true);
    } 
    if (counter == 3){
          sendTXmessage(TX_STATUS_4,true);
    }
    if (counter == 4){
          sendTXmessage(TX_KEEP_ALIVE,true);
    }
    if (counter >= 4){
          counter = 0;
          return;
    }                      
    counter++;
}  
                                                    


void w_bus::initSequence(void) 
{
    static int counter = 0;
    //if(waiting_for_rx_response == true) {DPRINTLN("waintn for RX!");return;}
    if(counter == 0) {
        sendTXmessage(TX_MESSAGE_INIT_1,true);
    }
    if (counter == 1){
        sendTXmessage(TX_MESSAGE_INIT_2,true);
    }
    if (counter == 2){
        sendTXmessage(TX_MESSAGE_INIT_3,true);
    } 
    if (counter == 3){
        sendTXmessage(TX_MESSAGE_INIT_4,false);
    }
    if (counter == 4){
        sendTXmessage(TX_MESSAGE_INIT_5,true);
    }
    if (counter == 5){
        sendTXmessage(TX_MESSAGE_INIT_6,true);
    }
    if (counter == 6){
        sendTXmessage(TX_MESSAGE_INIT_7,true);
    }
    if (counter == 7){
        sendTXmessage(TX_MESSAGE_INIT_8,true);
    }
    if (counter == 8){
        sendTXmessage(TX_MESSAGE_INIT_9,true);
    }                      
    if (counter == 9) {
        counter = 0;
        wbus_ok = true;
        return;
    }    
   counter++; 
}



void w_bus::printMsgDebug(void)
{
    DPRINTHEX(rx_msg.header);
    //DPRINT(' ');
    DPRINTHEX(rx_msg.length);
    //DPRINT(' ');
    for(int i=0;i<(rx_msg.length-1);i++) {
        DPRINTHEX(rx_msg.data[i]);
        //DPRINT(' ');
    }
    DPRINTHEX(rx_msg.checksum);
    DPRINTLN(); 
}


void w_bus::readSerialData(void)
{

    int rxByte = Serial1.read();
    
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

void w_bus::getSerialMessage(void) {

//Check that there are new bytes to read. If write_ptr != read_ptr data is ready to read.
if(write_ptr == read_ptr) return;


int rxByte = 0;

    
//Read data ONLY if we are in correct state...
if(!((rx_state == START) || (rx_state == RESET_STATE))) {
    rxByte = *read_ptr;
    //DPRINTLNHEX(rxByte);
    read_array_counter++;
    read_ptr++;

    // Reached end of buffer, wrap around.
    if(read_array_counter >= MESSAGE_BUFFER_SIZE) {
        read_array_counter = 0;
        read_ptr = &message_buffer[0];
    }
}

//Find message in data
  switch(rx_state) {
    case START:
        rx_state = FINDHEADER;
        break;  
    case FINDHEADER:
        //Below if statement could be flawed!!
        if((rxByte == TXHEADER) || (rxByte == RXHEADER)) {
                rx_state = READLENGTH;
                rx_msg.header = rxByte;
                //DPRINTLNHEX(rxByte);
        }
        break;
    //Header received
    case READLENGTH:
        //DPRINTLNHEX(rxByte);
        if((rxByte < 256) && (rxByte > 1)) {
            rx_state =READDATA;
            rx_msg.length=rxByte;
        }
        else {
            rx_state = RESET_STATE;
        }
        break;
    case READDATA:
        rx_msg.data[rx_msg.nr_data_read] = rxByte;
        rx_msg.data_string[rx_msg.nr_data_read] = (char)rxByte;
        rx_msg.nr_data_read++;
        if(rx_msg.nr_data_read >= rx_msg.length){
             int XOR = 0;
             XOR = XOR^rx_msg.header^rx_msg.length;
             //terminate data_string read
             rx_msg.data_string[rx_msg.nr_data_read]='\0';
             for(int i=0;i<(rx_msg.length-1);i++) {
                XOR = XOR ^ rx_msg.data[i];
             }
             rx_msg.checksum = rxByte;
             if(XOR == rx_msg.checksum) {
                rx_state = RESET_STATE;
                rx_msg.valid_message = true;
                //Print debug message
                #ifdef RAWDEBUGOUTPUT
                    printMsgDebug();
                #endif
                //Do parsing and shit if we received an RX message
                //if(rx_msg.header == RXHEADER) {
                    //calls message parser with global struct
                    //parseMessage();
                    //Clear TX response flag
                    //DPRINTLN("GOOD msg!!");
                    //waiting_for_rx_response = false;
                    //Clear timeout loops
                //}
             }
             else {
                rx_state = RESET_STATE;
                DPRINTLN("Checksum check FAILED!!");
             }
        }
        break;
    case RESET_STATE:
        rx_msg.header=0;
        rx_msg.length=0;
        rx_msg.nr_data_read=0;
        rx_msg.checksum=0;
        rx_msg.valid_message=false;
        rx_state=START;
        break;
    default:
        break;
  }




}
//*/


/*
void w_bus::readSerialData(void)
{

 //The reception of a message is implemented as a state machine 
 //Read is blocking. Wait until a message is read before doing anything else...
 //Need timeout? 
 
 //Do some checking if we are waiting for response and if we have had som timeouts
 //
 
 
 if(waiting_for_rx_response) {
   //delay(100);
   number_of_rx_loops++;
   if (number_of_rx_loops > 30){ 
       waiting_for_rx_response = false;

       DPRINTLN("Waiting for RX response timed!"); 
       time_out_loops++;
   }
   if(time_out_loops > 10) {
       DPRINTLN("Communication down on W-bus.. NO response");
       //Signal that wbus needs to be restarted
       wbus_ok = false;
   }
 }
*/
//
/*
  int rxByte = Serial1.read();
  //DPRINTLN(rxByte);
  switch(rx_state) {
    case START:
        rx_state = FINDHEADER;
        break;  
    case FINDHEADER:
        //DPRINTLNHEX(rxByte);
        //Below if statement could be flawed!!
        if((rxByte == TXHEADER) || (rxByte == RXHEADER)) {
                rx_state = READLENGTH;
                rx_msg.header = rxByte;
        }
        break;
    //Header received
    case READLENGTH:
        //DPRINTLNHEX(rxByte);
        if((rxByte < 256) && (rxByte > 1)) {
            rx_state =READDATA;
            rx_msg.length=rxByte;
        }
        else {
            rx_state = RESET_STATE;
        }
        break;
    case READDATA:
        rx_msg.data[rx_msg.nr_data_read] = rxByte;
        rx_msg.data_string[rx_msg.nr_data_read] = (char)rxByte;
        rx_msg.nr_data_read++;
        if(rx_msg.nr_data_read >= rx_msg.length){
             int XOR = 0;
             XOR = XOR^rx_msg.header^rx_msg.length;
             //terminate data_string read
             rx_msg.data_string[rx_msg.nr_data_read]='\0';
             for(int i=0;i<(rx_msg.length-1);i++) {
                XOR = XOR ^ rx_msg.data[i];
             }
             rx_msg.checksum = rxByte;
             if(XOR == rx_msg.checksum) {
                rx_state = RESET_STATE;
                rx_msg.valid_message = true;
                //Print debug message
                #ifdef RAWDEBUGOUTPUT
                    printMsgDebug();
                #endif
                //Do parsing and shit if we received an RX message
                if(rx_msg.header == RXHEADER) {
                    //calls message parser with global struct
                    //parseMessage();
                    //Clear TX response flag
                    //DPRINTLN("GOOD msg!!");
                    waiting_for_rx_response = false;
                    //Clear timeout loops
                    time_out_loops=0;
                }
             }
             else {
                rx_state = RESET_STATE;
                DPRINTLN("Checksum check FAILED!!");
             }
        }
        break;
    case RESET_STATE:
        rx_msg.header=0;
        rx_msg.length=0;
        rx_msg.nr_data_read=0;
        rx_msg.checksum=0;
        rx_msg.valid_message=false;
        rx_state=START;
        break;
    default:
        break;
    
 }

}*/ 

