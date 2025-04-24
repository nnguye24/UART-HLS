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
   // Set default configuration
   // these should be configured by a configuration register.
   parity_enabled = false;
   parity_even = true;  // Default to even parity
   
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
   
   // Reset error latched outputs
   parity_error_next = false;
   framing_error_next = false;
   overrun_error_next = false;
   
   // Reset actual outputs
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
   
   // Reset error outputs
   out_parity_error = false;
   out_framing_error = false;
   out_overrun_error = false;
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
   tx_start_next = false;
   tx_data_next = false;
   tx_parity_next = false;
   tx_stop_next = false;
   
   // State transition logic
   switch(tx_state.to_uint()) {
     case TX_IDLE:
       // Transition from IDLE state
       if (!tx_buffer_full) {
         tx_next_state = TX_LOAD;
         load_tx_next = true;
       } else {
         tx_next_state = TX_IDLE;
       }
       break;
       
     case TX_LOAD:
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
       // Transition based on bit count
       tx_data_next = true;
       if (tx_bit_counter >= 7) {  // All 8 data bits sent
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
       // Transition from STOP_BIT state
       tx_stop_next = true;
       if (!tx_buffer_full) {
         tx_next_state = TX_LOAD;  // More data to transmit
       } else {
         tx_next_state = TX_IDLE;  // Return to idle
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
   
   if (tx_state == TX_STOP_BIT) {
     tx_done = true;
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
   
   // Reset error flags
   parity_error_next = false;
   framing_error_next = false;
   overrun_error_next = false;
   
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
       // Transition from DATA_BITS state
       rx_data_next = true;
       
       // Store the current bit in the shift register
       rx_bit_value = rx_in;
       
       if (rx_bit_counter >= 7) {  // All 8 data bits received
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
         parity_error_next = true;
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
         framing_error_next = true;
         rx_next_state = RX_ERROR_HANDLING;
       } else {
         // Check for overrun condition
         if (!rx_buffer_empty) {
           // Previous data hasn't been read yet
           overrun_error_next = true;
           rx_next_state = RX_ERROR_HANDLING;
         } else {
           rx_next_state = RX_IDLE;
         }
       }
       break;
       
     case RX_ERROR_HANDLING:
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
   
   // Update error outputs
   out_parity_error = parity_error_next;
   out_framing_error = framing_error_next;
   out_overrun_error = overrun_error_next;
   
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
   
   if (rx_state == RX_STOP_BIT && rx_in == 1) {
     rx_done = true;
   } else {
     rx_done = false;
   }
 }
 
 // method to calculate parity
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