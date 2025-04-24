/**************************************************************
 * File Name: sizes.h
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/22/2025
 *
 * Contains the size definitions for the UART controller
 **************************************************************/

#ifndef __SIZES_H__
#define __SIZES_H__

// Defining the Sizes for the datapath
#define DATA_W 9       // 9-bit datapath for UART 
// Possibly DATA_W 9 for parity.
#define ADDR_W 6       // Address width to address each byte location(40 of them) 

// RAM and buffer sizes
#define TX_BUFFER_SIZE 16    // 16 bytes transmit buffer
#define RX_BUFFER_SIZE 16    // 16 bytes receive buffer
#define CONFIG_REG_SIZE 6    // 6 bytes of configuration registers
#define STATUS_REG_SIZE 2    // 2 bytes of status registers
#define RAM_SIZE 40          // Total RAM: 40 bytes (TX + RX + Config + Status)

// Number of nanoseconds in a cycle
#define CYCLE_LENGTH 5

// FSM state constants
#define IDLE 0
#define LOAD_TX 1
#define TX_START_BIT 2
#define TX_DATA_BITS 3
#define TX_PARITY_BIT 4
#define TX_STOP_BIT 5
#define RX_WAIT 6
#define RX_START_BIT 7
#define RX_DATA_BITS 8
#define RX_PARITY_CHECK 9
#define RX_STOP_BIT 10
#define ERROR_HANDLING 11

#endif