/**************************************************************
 * File Name: controller.cpp
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/22/2025
 *
 * UART Controller FSM implementation
 **************************************************************/

 #include "controller.h"
 #include <iostream>
 
 using namespace std;
 
 controller::controller() {
   reset();
 }
 
 // Reset method to initialize controller to default state
 void controller::reset() {
     // Initialize state to IDLE
     state.write(IDLE);
     
     // Reset all control outputs
     load_tx.write(false);
     tx_start.write(false);
     tx_data.write(false);
     tx_parity.write(false);
     tx_stop.write(false);
     rx_start.write(false);
     rx_data.write(false);
     rx_parity.write(false);
     rx_stop.write(false);
     error_handle.write(false);
     
     // Reset internal registers/counters
     bit_counter = 0;
     parity_value = 0;
     tx_done = false;
     rx_done = false;
 }
 
 // Next state logic implementation
 void controller::next_state_logic() {
     // Reset logic
     if (rst.read()) {
         reset();
         return;
     }
     
     // State transition logic
     switch(state.read()) {
         case IDLE:
             // Transition from IDLE state
             if (!tx_buffer_full.read()) {
                 state.write(LOAD_TX);
             } else if (rx_in.read() == 0) {
                 state.write(RX_START_BIT);
             }
             break;
             
         case LOAD_TX:
             // Transition from LOAD_TX state
             state.write(TX_START_BIT);
             bit_counter = 0;
             tx_done = false;
             break;
             
         case TX_START_BIT:
             // Transition from TX_START_BIT state
             state.write(TX_DATA_BITS);
             break;
             
         case TX_DATA_BITS:
             // Transition from TX_DATA_BITS state
             bit_counter++;
             if (bit_counter >= 8) {
                 // All 8 data bits sent
                 if (parity_enabled) {
                     state.write(TX_PARITY_BIT);
                 } else {
                     state.write(TX_STOP_BIT);
                 }
             }
             break;
             
         case TX_PARITY_BIT:
             // Transition from TX_PARITY_BIT state
             state.write(TX_STOP_BIT);
             break;
             
         case TX_STOP_BIT:
             // Transition from TX_STOP_BIT state
             tx_done = true;
             if (!tx_buffer_full.read()) {
                 state.write(LOAD_TX);  // More data to transmit
             } else {
                 state.write(IDLE);     // Return to idle
             }
             break;
             
         case RX_WAIT:
             // Transition from RX_WAIT state
             if (rx_in.read() == 0) {
                 state.write(RX_START_BIT);
             }
             break;
             
         case RX_START_BIT:
             // Transition from RX_START_BIT state
             state.write(RX_DATA_BITS);
             bit_counter = 0;
             parity_value = 0;
             rx_done = false;
             break;
             
         case RX_DATA_BITS:
             // Transition from RX_DATA_BITS state
             bit_counter++;
             if (bit_counter >= 8) {
                 // All 8 data bits received
                 if (parity_enabled) {
                     state.write(RX_PARITY_CHECK);
                 } else {
                     state.write(RX_STOP_BIT);
                 }
             }
             break;
             
         case RX_PARITY_CHECK:
             // Transition from RX_PARITY_CHECK state
             if (parity_error.read()) {
                 state.write(ERROR_HANDLING);
             } else {
                 state.write(RX_STOP_BIT);
             }
             break;
             
         case RX_STOP_BIT:
             // Transition from RX_STOP_BIT state
             if (rx_in.read() != 1) {
                 // Framing error - no stop bit detected
                 state.write(ERROR_HANDLING);
             } else {
                 rx_done = true;
                 state.write(IDLE);
             }
             break;
             
         case ERROR_HANDLING:
             // Transition from ERROR_HANDLING state
             // After error handling is done, return to idle
             state.write(IDLE);
             break;
             
         default:
             // Default case - return to IDLE
             state.write(IDLE);
             break;
     }
 }
 
 // Output logic implementation
 void controller::output_logic() {
     // Default values for all outputs
     load_tx.write(false);
     tx_start.write(false);
     tx_data.write(false);
     tx_parity.write(false);
     tx_stop.write(false);
     rx_start.write(false);
     rx_data.write(false);
     rx_parity.write(false);
     rx_stop.write(false);
     error_handle.write(false);
     
     // Output logic based on current state
     switch(state.read()) {
         case IDLE:
             // Output signals for IDLE state
             // No control signals active in IDLE state
             break;
             
         case LOAD_TX:
             // Output signals for LOAD_TX state
             load_tx.write(true);
             break;
             
         case TX_START_BIT:
             // Output signals for TX_START_BIT state
             tx_start.write(true);
             break;
             
         case TX_DATA_BITS:
             // Output signals for TX_DATA_BITS state
             tx_data.write(true);
             break;
             
         case TX_PARITY_BIT:
             // Output signals for TX_PARITY_BIT state
             tx_parity.write(true);
             break;
             
         case TX_STOP_BIT:
             // Output signals for TX_STOP_BIT state
             tx_stop.write(true);
             break;
             
         case RX_WAIT:
             // Output signals for RX_WAIT state
             // No specific control signals
             break;
             
         case RX_START_BIT:
             // Output signals for RX_START_BIT state
             rx_start.write(true);
             break;
             
         case RX_DATA_BITS:
             // Output signals for RX_DATA_BITS state
             rx_data.write(true);
             break;
             
         case RX_PARITY_CHECK:
             // Output signals for RX_PARITY_CHECK state
             rx_parity.write(true);
             break;
             
         case RX_STOP_BIT:
             // Output signals for RX_STOP_BIT state
             rx_stop.write(true);
             break;
             
         case ERROR_HANDLING:
             // Output signals for ERROR_HANDLING state
             error_handle.write(true);
             break;
             
         default:
             // Default case - no control signals active
             break;
     }
 }