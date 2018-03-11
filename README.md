# webasto

The goal of this project is the following:

1) Implement HW that can talk to the webasto implemented W-bus
2) Implement SW that sniffs the webasto traffic
3) Implement SW that can talk to the webasto

The arduino UNO was selected together with a raspberry pi (for remote development) was selected in this project.

W-bus protocol
-------------

The W-bus protocol is a bidirectional link using single physical line for communication. On the physical layer it uses 0 -> 12V signaling. THe w-bus is a K-line interface. The K-line interface is descriobed in ISO 9141.

