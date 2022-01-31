// BTCP USAGE EXAMPLE
// USING BSSR GEN11 TCP PROTOCOL
// These are just code fragments, copy and paste them into appropriate spots in program.
// some code is taken directly from bssr's gen11 MCMB code, make sure to check that out once you understand how to transmit/receive data thru btcp

/*===================== SETTING THINGS UP =====================*/

// MAKE SURE btcp.c AND buart.c FILES ARE IN /Src DIRECTORY.
// MAKE SURE protocol_ids.h, btcp.h AND buart.h FILES ARE IN /Inc DIRECTORY.


/* Includes */
// INCLUDE THESE TWO HEADERS IN ORDER TO USE BSSR'S TCP PROTOCOL
#include "buart.h"
#include "btcp.h"
#include "protocol_ids.h" //Not truely necessary, already included by "btcp.h", but included here for clarity

/* Private Defines */
// for bitwise operations later on
#define MOTOR 16
#define FWD_REV 8
#define VFM_UP 4
#define VFM_DOWN 2
#define VFM_RESET 1

/* Private Variables */
//These three lines can be autogenerated by CubeMX
CRC_HandleTypeDef hcrc;
UART_HandleTypeDef huartA;
UART_HandleTypeDef huartB;

// TO TRANSMIT USING BSSR TCP PROTOCOL,
B_uartHandle_t *rxBuart;
B_uartHandle_t *txBuart;
B_tcpHandle_t *btcp;


/* Initialization */
// MUST INITIALIZE THESE HANDLES BEFORE TRANSMITTING ANYTHING USING THEM.
// this will create uart tx and rx tasks that will always be running throughout the program.
Buart = B_uartStart(&huartA);


// Next, initialize the btcp handle using B_tcpStart
// This will also create a Task that will be constantly looking for received data
// First parameter can be replaced with a pointer to an array of txBuarts if you want to transmit from multiple uart pins at once
btcp = B_tcpStart(/*transmitBuarts=*/ &Buart , /*rxBuart=*/ Buart , /*numTransmitBuarts=*/ 1, &hcrc);

/* Main Program/Default Task */
/*===================== SENDING DATA =====================*/
// As an example, let the sender be the MCMB
// First we need to define the data that is to be sent.
// data buffer can be up to 255 bytes long. we'll transmit 4 bytes in this case. easiest way to transmit is to define an array of bytes.
// Adaped from GEN11 MCMB
uint8_t buf[4] = {0x00, 0x00, 0x00, 0x00};

// according to the protocol, the first byte of the payload must be the data identifier. This helps specify what kind of data is in the payload.
// refer to Gen11 socket protocol for details on which data ID and sender address to use.
// For readability, use the ENUMS defined in protocol_ids.h
buf[0] = MCMB_SPEED_PULSE_ID; // data ID for MCMB's speed-reading, as specified in protocol, assuming that it's the MCMB that's sending the data...
															// Note that MCMB_SPEED_PULSE_ID is equivalent to 0x01 as written in protocol

// fill the rest of the bytes with whatever useful data you want to send.
buf[1] = carSpeed;  
buf[2] = 0x00; // Nothing useful to send here
buf[3] = 0x00;

// Now that payload has the data we want to send, call B_tcpSend() to transmit data
// 1st parameter is pointer B_tcpHandle_t. 
// 2nd parameter is pointer to buffer (uint8_t array). 
// 3rd parameter is a uint8_t whose value is how many bytes of data we are sending.
B_tcpSend(btcp, buf, sizeof(buf));

/*===================== RECEIVING DATA =====================*/
// Let the receiver be the DCMB, who is to receive from the sender (MCMB) as well as other boards
// For receiving data, you need to define a function called serialParse() that takes a B_tcpPacket_t* as its only parameter, like so:
// In this function is where you will store data from the received tcpPacket to variables of your choice.
// This function is a callback function that will be automatically called by the btcp Rx task
// Adapted from main.c of Gen11 DCMBfirmware
// define some variables that will hold the important data you need
// for receiving data only.
uint8_t carSpeed = 0;

void serialParse(B_tcpPacket_t *pkt){
	switch(pkt->senderID){ // decide what to do based on what sender address is received
			case MCMB_ID:  // If the packet comes from MCMB
					if(pkt->data[0] == MCMB_SPEED_PULSE_ID){
							carSpeed = pkt->data[1];
					}else if(pkt->data[0] == MCMB_MOTOR_TEMPERATURE_ID) {
							// load temperature into local variable
					}else if (pkt->data[0] == MCMB_BUS_METRICS_ID) {
							// Process Bus Metrics
					}
			case BBMB_ID: // If the packed comes from BBMB
					// Same as before, use if statements to check for Data IDs
	}
}