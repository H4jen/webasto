# webasto
Mostly because my webasto air top evo 40 broke down and I was told to send the unit back to repair shop for SW reset in order to get it working again. I decided to put some effort into understanding and hopefully be able to controll the heater without sending things back to repair shop in the future.

The goal of this project is the following:

1) Implement HW interface that can talk to the webasto implemented W-bus (K-line) using arduino HW and simple HW TTL/K-line interface.
2) Implement SW that can sniff the webasto traffic
3) Implement SW that can talk to the webasto
4) Map protocol used to communicate with my webasto air top evo 40. May be scalable to other versions??
5) Build diagnostic box with status LCD display that monitors the Webasto heater.

The arduino UNO was selected together with a raspberry pi (for remote development) was selected in this project.

W-bus protocol
-------------

The W-bus protocol is a bi-directional link using single physical line for communication. On the physical layer it uses 0 -> 12V signaling. The w-bus is a K-line interface. The K-line interface is described in ISO 9141. Normally the K-line is interfaced using integrated chips. But is this project a simple transistor cascade was selected. The K-line is held in high state (Vbat).

HW interface
------------
THe HW interface is simulated using LTspice from linear tech, which is an excellent freeware implmentation of SPICE. The HW interface is a TTL to W-bus (K-line) converter using standard resistors and NPN transistors.

![HW_interface_Wbus](HW_interface_Wbus.JPG)

There are some open questions on the input resistance of the W-bus interface of the webasto and the current drive capabilities. The TX output resistor and RX input resistor should be scale accordingly to this.
The above circuit was simulated. The resulting curves with TX TTL baudrate 9600 shown at the W-bus output (12V) and the resulting curve at the RX TTL interface (+5v).

![RX_TTL_input_W-BUS_output.JPG](RX_TTL_input_W-BUS_output.JPG)
