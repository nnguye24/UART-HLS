/**************************************************************
 * File Name: controller.cpp
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/22/2025
 *
 * Full-duplex UART Controller implementation with error detection
 **************************************************************/

 #include "controller.h"
 #include <iostream>
 
 using namespace std;
 
 controller::controller() {
   // Initialize controller
   reset();
 }
 
 void controller::reset() {
   // Reset TX state
   tx_state = TX_IDLE;
   tx_next_state = TX_IDLE;
   tx_bit_counter = 0;
   tx_parity_value = false;
   tx_done = false;
   tx_data_register = false;
   
   // Reset RX state
   rx_state = RX_IDLE;
   rx_next_state = RX_IDLE;
   rx_bit_counter = 0;
   rx_parity_value = false;
   rx_done = false;
   rx_shift_reg = 0;
   rx_bit_value = false;
   
   // Reset TX latched outputs
   load_tx_next = false;
   load_tx2_next = false
   tx_start_next = false;
   tx_data_next = false;
   tx_parity_next = false;
   tx_stop_next = false;
   
   // Reset RX latched outputs
   rx_start_next = false;
   rx_data_next = false;
   rx_parity_next = false;
   rx_stop_next = false;
   error_handle_next = false;
   
   // Reset actual outputs
   out_load_tx2 = false;
   out_load_tx = false;
   out_tx_start = false;
   out_tx_data = false;
   out_tx_parity = false;
   out_tx_stop = false;
   out_rx_start = false;
   out_rx_data = false;
   out_rx_parity = false;
   out_rx_stop = false;
   out_error_handle = false;
   out_rx_read = false;
 }
 
 void controller::compute() {
   // Reset logic
   if (rst) {
     reset();
     return;
   }
   
   // Process TX and RX independently
   compute_tx();
   compute_rx();
 }
 
 void controller::commit() {
   // Update TX and RX states independently
   commit_tx();
   commit_rx();
 }
 
 void controller::compute_tx() {
   // Reset all next-cycle outputs to default
   load_tx_next = false;
   load_tx2_next = false;
   tx_start_next = false;
   tx_data_next = false;
   tx_parity_next = false;
   tx_stop_next = false;
   
   // State transition logic
   switch(tx_state.to_uint()) {
     case TX_IDLE:
       // Transition from IDLE state
       if (!tx_buffer_full) {
         tx_next_state = LOAD_TX;
         load_tx_next = true;
       } else {
         tx_next_state = TX_IDLE;
       }
       break;
       
     case LOAD_TX:
       // Transition from LOAD state
       tx_next_state = LOAD_TX2;
       load_tx2_next = true;
       break;

     case LOAD_TX2:
       // Transition from LOAD state
       tx_next_state = TX_START_BIT;
       tx_start_next = true;
       break;  
       
     case TX_START_BIT:
       // Transition from START_BIT state
       tx_next_state = TX_DATA_BITS;
       tx_data_next = true;
       break;
       
     case TX_DATA_BITS:
       // Transition based on bit count and configured data bits
       tx_data_next = true;
       if (tx_bit_counter >= data_bits - 1) {  // Adjust for configured data bits
         if (parity_enabled) {
           tx_next_state = TX_PARITY_BIT;
         } else {
           tx_next_state = TX_STOP_BIT;
         }
       } else {
         tx_next_state = TX_DATA_BITS;
       }
       break;
       
     case TX_PARITY_BIT:
       // Transition from PARITY_BIT state
       tx_parity_next = true;
       tx_next_state = TX_STOP_BIT;
       break;
       
     case TX_STOP_BIT:
       // Transition from STOP_BIT state - handle multiple stop bits if configured
       tx_stop_next = true;
       
       // Check if we need a second stop bit
       if (stop_bits == 2 && tx_bit_counter == 0) {
         tx_bit_counter = 1;  // Mark that we're sending the second stop bit
         tx_next_state = TX_STOP_BIT;  // Stay in stop bit state
       } else {
         // Done with all stop bits
         if (!tx_buffer_full) {
           tx_next_state = TX_LOAD;  // More data to transmit
         } else {
           tx_next_state = TX_IDLE;  // Return to idle
         }
       }
       break;
       
     default:
       // Default case - return to IDLE
       tx_next_state = TX_IDLE;
       break;
   }
 }
 
 void controller::commit_tx() {
   // Update TX state
   tx_state = tx_next_state;
   
   // Update TX outputs
   out_load_tx = load_tx_next;
   out_load_tx2 = load_tx2_next;
   out_tx_start = tx_start_next;
   out_tx_data = tx_data_next;
   out_tx_parity = tx_parity_next;
   out_tx_stop = tx_stop_next;
   
   // Update TX internal counters/flags
   if (tx_state == TX_DATA_BITS) {
     tx_bit_counter++;
   }
   
   if (tx_state == TX_LOAD) {
     tx_bit_counter = 0;
   }
   
   if (tx_state == TX_STOP_BIT && tx_next_state != TX_STOP_BIT) {
     tx_done = true;
     tx_bit_counter = 0;  // Reset counter for next transmission
   } else {
     tx_done = false;
   }
 }
 
 void controller::compute_rx() {
   // Reset all next-cycle outputs to default
   rx_start_next = false;
   rx_data_next = false;
   rx_parity_next = false;
   rx_stop_next = false;
   error_handle_next = false;
   
   // State transition logic
   switch(rx_state.to_uint()) {
     case RX_IDLE:
       // Transition from IDLE state - wait for start bit
       if (rx_in == 0) {
         rx_next_state = RX_START_BIT;
         rx_start_next = true;
       } else {
         rx_next_state = RX_IDLE;
       }
       break;
       
     case RX_START_BIT:
       // Transition from START_BIT state
       rx_start_next = true;
       rx_next_state = RX_DATA_BITS;
       // Check for valid start bit (should be 0)
       if (rx_in != 0) {
         // False start bit detected, go back to IDLE
         rx_next_state = RX_IDLE;
       }
       break;
       
     case RX_DATA_BITS:
       // Transition from DATA_BITS state based on configured data bits
       rx_data_next = true;
       
       // Store the current bit in the shift register
       rx_bit_value = rx_in;
       
       if (rx_bit_counter >= data_bits - 1) {  // Adjust for configured data bits
         // Calculate expected parity for received data
         rx_parity_value = calculate_parity(rx_shift_reg);
         
         if (parity_enabled) {
           rx_next_state = RX_PARITY_CHECK;
         } else {
           rx_next_state = RX_STOP_BIT;
         }
       } else {
         rx_next_state = RX_DATA_BITS;
       }
       break;
       
     case RX_PARITY_CHECK:
       // Transition from PARITY_CHECK state
       rx_parity_next = true;
       
       // Check if received parity matches calculated parity
       if ((rx_in && !rx_parity_value) || (!rx_in && rx_parity_value)) {
         // Parity error detected
         rx_next_state = RX_ERROR_HANDLING;
       } else {
         rx_next_state = RX_STOP_BIT;
       }
       break;
       
     case RX_STOP_BIT:
       // Transition from STOP_BIT state
       rx_stop_next = true;
       
       // Check for valid stop bit (should be 1)
       if (rx_in != 1) {
         // Framing error - no stop bit detected
         rx_next_state = RX_ERROR_HANDLING;
       } else {
         // Check for second stop bit if configured
         if (stop_bits == 2 && rx_bit_counter == 0) {
           rx_bit_counter = 1;  // Mark that we're checking the second stop bit
           rx_next_state = RX_STOP_BIT;  // Stay in stop bit state
         } else {
           // All stop bits received correctly
           rx_next_state = RX_IDLE;
           
           // Signal to read the received data
           out_rx_read = true;
         }
       }
       break;
       
     case ERROR_HANDLING:
       // Transition from ERROR_HANDLING state
       error_handle_next = true;
       rx_next_state = RX_IDLE;
       break;
       
     default:
       // Default case - return to IDLE
       rx_next_state = RX_IDLE;
       break;
   }
 }
 
 void controller::commit_rx() {
   // Update RX state
   rx_state = rx_next_state;
   
   // Update RX outputs
   out_rx_start = rx_start_next;
   out_rx_data = rx_data_next;
   out_rx_parity = rx_parity_next;
   out_rx_stop = rx_stop_next;
   out_error_handle = error_handle_next;
   
   // Update RX internal counters/flags
   if (rx_state == RX_DATA_BITS) {
     // Shift in the received bit
     rx_shift_reg = (rx_shift_reg << 1) | (rx_bit_value ? 1 : 0);
     rx_bit_counter++;
   }
   
   if (rx_state == RX_START_BIT) {
     rx_bit_counter = 0;
     rx_shift_reg = 0;
   }
   
   if (rx_state == RX_STOP_BIT && rx_next_state == RX_IDLE) {
     rx_done = true;
   } else {
     rx_done = false;
   }
 }
 
 bool controller::calculate_parity(sc_bv<8> data) {
   int count = 0;
   
   // Count the number of '1' bits
   for (int i = 0; i < 8; i++) {
     if (data[i] == '1') {
       count++;
     }
   }
   
   // For even parity: return true if count is odd (need to add a 1)
   // For odd parity: return true if count is even (need to add a 1)
   if (parity_even) {
     return (count % 2 != 0);  // Even parity
   } else {
     return (count % 2 == 0);  // Odd parity
   }
 }