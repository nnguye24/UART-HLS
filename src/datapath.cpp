/**************************************************************
 * File Name: datapath.cpp
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/22/2025
 *
 * Datapath implementation for UART controller
 **************************************************************/

 #include "datapath.h"
 #include <iostream>
 
 using namespace std;
 
 // Memory map address definitions
 #define TX_BUFFER_START     0   // 16 bytes (0-15)
 #define RX_BUFFER_START    16   // 16 bytes (16-31)
 #define CONFIG_REG_START   32   // 6 bytes (32-37)
 #define STATUS_REG_START   38   // 2 bytes (38-39)
 
 // Configuration register addresses
 #define BAUD_RATE_LOW      32   // Baud rate divisor (low byte)
 #define BAUD_RATE_HIGH     33   // Baud rate divisor (high byte)
 #define LINE_CONTROL_REG   34   // Line control register
 #define FIFO_CONTROL_REG   35   // FIFO control register
 #define SCRATCH_REG1       36   // Scratch register 1
 #define SCRATCH_REG2       37   // Scratch register 2
 
 // Line control register bit definitions
 #define LCR_DATA_BITS_MASK 0x03 // Bits 0-1: Data bits (0=5, 1=6, 2=7, 3=8)
 #define LCR_STOP_BITS      0x04 // Bit 2: Stop bits (0=1, 1=2)
 #define LCR_PARITY_ENABLE  0x08 // Bit 3: Parity enable
 #define LCR_PARITY_EVEN    0x10 // Bit 4: Even parity when set, odd when clear
 #define LCR_STICK_PARITY   0x20 // Bit 5: Stick parity
 #define LCR_BREAK_CONTROL  0x40 // Bit 6: Break control
 #define LCR_DLAB           0x80 // Bit 7: Divisor latch access bit
 
 datapath::datapath() {
     reset();
 }
 
 void datapath::reset() {
     // Reset transmit and receive registers
     tx_shift_register = 0;
     rx_shift_register = 0;
     
     // Reset buffer pointers
     tx_buf_head = 0;
     tx_buf_tail = 0;
     rx_buf_head = 0;
     rx_buf_tail = 0;
     
     // Reset bit counters
     tx_bit_count = 0;
     rx_bit_count = 0;
     
     // Reset output signals
     tx_out = 1;  // Idle state is high
     tx_buffer_full = false;
     rx_buffer_empty = true;
     parity_error = false;
     framing_error = false;
     overrun_error = false;
     data_out = 0;
     
     // Reset baud rate generation
     baud_divider = 0x0003;  // Default baud rate divisor
     baud_counter = 0;
     
     // Reset configuration
     parity_enabled = false;
     parity_even = true;
     data_bits = 8;
     stop_bits = 1;
     
     // Initialize buffers to zero
     for (int i = 0; i < TX_BUFFER_SIZE; i++) {
         tx_buffer[i] = 0;
     }
     for (int i = 0; i < RX_BUFFER_SIZE; i++) {
         rx_buffer[i] = 0;
     }
     
     // Reset internal next-state values
     next_tx_buffer_full = false;
     next_tx_out = 1;
     next_tx_shift_register = 0;
     next_rx_buffer_empty = true;
     next_rx_buf_head = 0;
     next_rx_buf_tail = 0;
     next_parity_error = false;
     next_framing_error = false;
     next_overrun_error = false;
     next_rx_shift_register = 0;
     next_data_out = 0;
 }
 
 void datapath::compute() {
     // Reset logic
     if (rst) {
         reset();
         return;
     }
     
     // First, update configuration from memory map
     update_configuration();
     
     // Process TX and RX independently
     compute_tx();
     compute_rx();
 }
 
 // New method to read and interpret configuration registers
 void datapath::update_configuration() {
     // Request configuration register values by setting address
     addr = LINE_CONTROL_REG;
     
     // Read line control register - in real hardware this would have a delay
     sc_uint<DATA_W> lcr = data_in;
     
     // Extract configuration parameters
     data_bits = (lcr & LCR_DATA_BITS_MASK) + 5;  // Convert to actual number (5-8)
     stop_bits = ((lcr & LCR_STOP_BITS) != 0) ? 2 : 1;
     parity_enabled = (lcr & LCR_PARITY_ENABLE) != 0;
     parity_even = (lcr & LCR_PARITY_EVEN) != 0;
     
     // Read baud rate divisor
     addr = BAUD_RATE_LOW;
     sc_uint<DATA_W> baud_low = data_in;
     
     addr = BAUD_RATE_HIGH;
     sc_uint<DATA_W> baud_high = data_in;
     
     // Combine to form 16-bit baud rate divisor
     baud_divider = (baud_high << 8) | baud_low;
 }
 
 // TX compute method
 void datapath::compute_tx() {
     // Reset TX next-state values to defaults
     next_tx_buffer_full = tx_buffer_full;
     next_tx_out = tx_out;
     next_tx_shift_register = tx_shift_register;
     
     // Handle TX operations based on control signals
     if (load_tx) {
         // Load TX shift register from buffer
         load_tx_register();
     }
     
     if (tx_start) {
         // Send start bit (always 0)
         next_tx_out = false;
     }
     
     if (tx_data) {
         // Send data bits (LSB first)
         next_tx_out = tx_shift_register[0];
         next_tx_shift_register = tx_shift_register >> 1;
     }
     
     if (tx_parity && parity_enabled) {
         // Send parity bit if enabled
         next_tx_out = calculate_parity(tx_shift_register);
     }
     
     if (tx_stop) {
         // Send stop bit (always 1)
         next_tx_out = true;
     }
     
     // Update tx_buffer_full status
     next_tx_buffer_full = tx_buffer_check();
 }
 
 // RX compute method
 void datapath::compute_rx() {
     // Initialize next-state values to current values
     next_rx_buffer_empty = rx_buffer_empty;
     next_parity_error = parity_error;
     next_framing_error = framing_error;
     next_overrun_error = overrun_error;
     next_rx_shift_register = rx_shift_register;
     next_rx_buf_head = rx_buf_head;
     next_rx_buf_tail = rx_buf_tail;
     next_data_out = data_out;
     
     // Handle RX operations based on control signals
     if (rx_start) {
         // Verify start bit is 0
         if (rx_in != 0) {
             next_framing_error = true;
         }
     }
     
     if (rx_data) {
         // Receive data bit - shift from MSB down to match the transmission order (LSB first)
         next_rx_shift_register = (rx_shift_register >> 1);
         // Place new bit in MSB position
         next_rx_shift_register[DATA_W-1] = rx_in;
     }
     
     if (rx_parity && parity_enabled) {
         // Check parity if enabled
         bool expected_parity = calculate_parity(rx_shift_register);
         if (rx_in != expected_parity) {
             next_parity_error = true;
         }
     }
     
     if (rx_stop) {
         // Verify stop bit is 1
         if (rx_in != 1) {
             next_framing_error = true;
         }
         // Check for buffer overrun before storing
         else if (((rx_buf_head + 1) % RX_BUFFER_SIZE) == rx_buf_tail) {
             // Buffer full → flag overrun, don't store new byte
             next_overrun_error = true;
         }
         else if (!next_framing_error && !(parity_enabled && next_parity_error)) {
             // Only store data if no errors
             store_rx_register();
         }
     }
     
     if (error_handle) {
         // Handle error conditions - clear error flags
         next_parity_error = false;
         next_framing_error = false;
         next_overrun_error = false;
     }
     
     // CPU/host is reading one byte:
     if (rx_read && !rx_buffer_empty) {
         next_data_out = rx_buffer[rx_buf_tail];
         next_rx_buf_tail = (rx_buf_tail + 1) % RX_BUFFER_SIZE;
     }
 
     // Recompute empty flag
     next_rx_buffer_empty = (next_rx_buf_head == next_rx_buf_tail);
 }
 
 // Commit methods update actual registers based on next-state values
 void datapath::commit() {
     commit_tx();
     commit_rx();
 }
 
 void datapath::commit_tx() {
     // Update TX outputs and registers
     tx_buffer_full = next_tx_buffer_full;
     tx_out = next_tx_out;
     tx_shift_register = next_tx_shift_register;
     
     // Update TX bit counter
     if (tx_data) {
         tx_bit_count++;
         if (tx_bit_count >= data_bits) {
             tx_bit_count = 0;
         }
     }
     
     // Reset bit counter when loading new data
     if (load_tx) {
         tx_bit_count = 0;
     }
 }
 
 void datapath::commit_rx() {
     // Update RX outputs and registers
     rx_buffer_empty = next_rx_buffer_empty;
     parity_error = next_parity_error;
     framing_error = next_framing_error;
     overrun_error = next_overrun_error;
     rx_shift_register = next_rx_shift_register;
     rx_buf_head = next_rx_buf_head;
     rx_buf_tail = next_rx_buf_tail;
     data_out = next_data_out;
     
     // Update RX bit counter
     if (rx_data) {
         rx_bit_count++;
     }
     
     if (rx_start) {
         rx_bit_count = 0;
     }
 }
 
 // Helper methods
 
 bool datapath::calculate_parity(sc_bv<8> data) {
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
 
 bool datapath::tx_buffer_check() {
     // Check if TX buffer is full
     return ((tx_buf_head + 1) % TX_BUFFER_SIZE) == tx_buf_tail;
 }
 
 bool datapath::rx_buffer_check() {
     // Check if RX buffer is empty
     return rx_buf_head == rx_buf_tail;
 }
 
 void datapath::load_tx_register() {
     // Load data from TX buffer to shift register
     if (tx_buf_head != tx_buf_tail) {
         tx_shift_register = tx_buffer[tx_buf_tail];
         tx_buf_tail = (tx_buf_tail + 1) % TX_BUFFER_SIZE;
     }
 }
 
 void datapath::store_rx_register() {
     // Store data from shift register to RX buffer
     // Only store the relevant bits based on data_bits configuration
     sc_bv<8> masked_data = rx_shift_register;
     
     // Apply mask based on data_bits (if less than 8)
     if (data_bits < 8) {
         sc_bv<8> mask = (1 << data_bits) - 1;
         masked_data &= mask;
     }
     
     rx_buffer[rx_buf_head] = masked_data;
     next_rx_buf_head = (rx_buf_head + 1) % RX_BUFFER_SIZE;
 }
 
 // New method to provide configuration to controller 
 void datapath::sync_controller_config() {
     // Set signals that controller needs to know about
     ctrl_parity_enabled = parity_enabled;
     ctrl_parity_even = parity_even;
     ctrl_data_bits = data_bits;
     ctrl_stop_bits = stop_bits;
 }