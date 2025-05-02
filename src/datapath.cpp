/**************************************************************
 * File Name: datapath.cpp
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/22/2025
 *
 * Datapath implementation for UART controller
 **************************************************************/

 #include "datapath.h"
 
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
 
 void datapath::process() {
    {
        HLS_DEFINE_PROTOCOL("reset");
        reset_datapath_clear_regs();
    }
    {
        HLS_DEFINE_PROTOCOL("get_config");
        get_config();
    }
     
    {       
        HLS_DEFINE_PROTOCOL("wait");
        wait();
    }
    {
        HLS_DEFINE_PROTOCOL("update");
        update_configuration();
    }
     
    while(true) {
        {       
            HLS_DEFINE_PROTOCOL("wait");
            in_start = start.read();
            wait();
        }
        {
            HLS_DEFINE_PROTOCOL("read_mem_we");
      
            // Read in the 
            in_mem_we = mem_we.read(); 
        }
         
         // Check if memory write is active
        if(!in_mem_we) {
                {
                    HLS_DEFINE_PROTOCOL("start");
                    reset_datapath_clear_regs();
                }
            } else {
                {
                    HLS_DEFINE_PROTOCOL("input");
                    read_inputs();
                }

                {
                    HLS_DEFINE_PROTOCOL("datapath_ops");
                    compute();
                }
        }
        {
            HLS_DEFINE_PROTOCOL("outputs");
            write_outputs();
        }
         
         // Wait for next cycle
        {       
             HLS_DEFINE_PROTOCOL("wait");
             wait();
        }
    }
}
 
 void datapath::reset_datapath_clear_regs() {
     load_tx_phase = false;
 
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
     out_tx_out = 1;  // Idle state is high
     out_tx_buffer_full = false;
     out_rx_buffer_empty = true;
     out_parity_error = false;
     out_framing_error = false;
     out_overrun_error = false;
     out_data_out = 0;
     out_dp_write_enable = false;
     
     // Reset baud rate generation
     out_baud_divider = 0x0003;  // Default baud rate divisor
     
     // Reset configuration
     parity_enabled = false;
     parity_even = true;
     data_bits = 8;
     stop_bits = 1;
     
     // Reset controller config outputs
     out_ctrl_parity_enabled = false;
     out_ctrl_parity_even = true;
     out_ctrl_data_bits = 8;
     out_ctrl_stop_bits = 1;
     
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
 
 void datapath::read_inputs() {
     // Read all input ports
     // in_start = start.read();
     in_mem_we = mem_we.read();
     in_load_tx = load_tx.read();
     in_load_tx2 = load_tx2.read();
     in_tx_start = tx_start.read();
     in_tx_data = tx_data.read();
     in_tx_parity = tx_parity.read();
     in_tx_stop = tx_stop.read();
     in_rx_start = rx_start.read();
     in_rx_data = rx_data.read();
     in_rx_parity = rx_parity.read();
     in_rx_stop = rx_stop.read();
     in_error_handle = error_handle.read();
     in_rx_read = rx_read.read();
     in_rx_in = rx_in.read();
     in_data_in = data_in.read();
 }
 
 void datapath::write_outputs() {
     // Write to all output ports
     tx_buffer_full.write(out_tx_buffer_full);
     rx_buffer_empty.write(out_rx_buffer_empty);
     parity_error.write(out_parity_error);
     framing_error.write(out_framing_error);
     overrun_error.write(out_overrun_error);
     ctrl_parity_enabled.write(out_ctrl_parity_enabled);
     ctrl_parity_even.write(out_ctrl_parity_even);
     ctrl_data_bits.write(out_ctrl_data_bits);
     ctrl_stop_bits.write(out_ctrl_stop_bits);
     tx_out.write(out_tx_out);
     data_out.write(out_data_out);
     addr.write(out_addr);
     dp_data_in.write(out_dp_data_in);
     dp_addr.write(out_dp_addr);
     dp_write_enable.write(out_dp_write_enable);
     baud_divisor.write(out_baud_divider);  // Added baud_divisor output
 }
 
 void datapath::compute() {
     // Reset logic
     if (rst.read()) {
         reset_datapath_clear_regs();
         return;
     }
     
     // First, update configuration from memory map
    //  get_config();
    //  update_configuration();    // this placement might be a problem
    //  // we want to constantly update the configuration so that it is up to date.
     
     // Process TX and RX independently
     compute_tx();
     compute_rx();
 }
 

 void datapath::get_config(){
    // sends out config address to request data from that register
    // in_data_in should change in the next clock cycle(read)
    out_addr = LINE_CONTROL_REG;

 }
 // New method to read and interpret configuration registers
 void datapath::update_configuration() {
     // Read line control register
     sc_uint<DATA_W> lcr = in_data_in;
     
     // Extract configuration parameters
     data_bits = (lcr & LCR_DATA_BITS_MASK) + 5;  // Convert to actual number (5-8)
     stop_bits = ((lcr & LCR_STOP_BITS) != 0) ? 2 : 1;
     parity_enabled = (lcr & LCR_PARITY_ENABLE) != 0;
     parity_even = (lcr & LCR_PARITY_EVEN) != 0;
     
     // Update controller configuration outputs
     out_ctrl_parity_enabled = parity_enabled;
     out_ctrl_parity_even = parity_even;
     out_ctrl_data_bits = data_bits;
     out_ctrl_stop_bits = stop_bits;
     
     // Read baud rate divisor
     out_addr = BAUD_RATE_LOW;
     sc_uint<DATA_W> baud_low = in_data_in;
     
     out_addr = BAUD_RATE_HIGH;
     sc_uint<DATA_W> baud_high = in_data_in;
     
     // Combine to form 16-bit baud rate divisor
     out_baud_divider = (baud_high << 8) | baud_low;
     // Baud_divider is what out baud counter counts up to
     // currently don't have a baud counter. 
 }
 
 // TX compute method
 void datapath::compute_tx() {
     // Reset TX next-state values to defaults
     next_tx_buffer_full = out_tx_buffer_full;
     next_tx_out = out_tx_out;
     next_tx_shift_register = tx_shift_register;
     
     // Handle TX operations based on control signals
     if (in_load_tx) {
         if (tx_buf_head != tx_buf_tail) {
             if (!load_tx_phase) {
                 // First phase: Set up the memory address
                 unsigned int mem_addr = tx_buf_tail; // TX buffer starts at address 0
                 out_addr = mem_addr; // Set address to read from Memory
                 load_tx_phase = true; // Move to second phase next time
             }
         }
     }
 
     if (in_load_tx2) {
         next_tx_shift_register = in_data_in;
         tx_buf_tail = (tx_buf_tail + 1) % TX_BUFFER_SIZE;
         load_tx_phase = false; // Reset phase for next load operation
     }
      
     if (in_tx_start) {
         // Send start bit (always 0)
         next_tx_out = false;
     }
      
     if (in_tx_data) {
         // Send data bits (LSB first)
         next_tx_out = tx_shift_register[0];    // tx_out is a TX pin
         next_tx_shift_register = tx_shift_register >> 1;
     }
      
     if (in_tx_parity && parity_enabled) {
         // Send parity bit if enabled
         next_tx_out = calculate_parity(tx_shift_register);
     }
      
     if (in_tx_stop) {
         // Send stop bit (always 1)
         next_tx_out = true;
     }
      
     // Update tx_buffer_full status
     next_tx_buffer_full = tx_buffer_check();
 }
 
 // RX compute method
 void datapath::compute_rx() {
     // Initialize next-state values to current values
     next_rx_buffer_empty = out_rx_buffer_empty;
     next_parity_error = out_parity_error;
     next_framing_error = out_framing_error;
     next_overrun_error = out_overrun_error;
     next_rx_shift_register = rx_shift_register;
     next_rx_buf_head = rx_buf_head;
     next_rx_buf_tail = rx_buf_tail;
     next_data_out = out_data_out;
     
     // Handle RX operations based on control signals
     if (in_rx_start) {
         // Verify start bit is 0
         if (in_rx_in != 0) {
             next_framing_error = true;
         }
     }
      
     if (in_rx_data) {
         // Receive data bit - shift from MSB down to match the transmission order (LSB first)
         next_rx_shift_register = (rx_shift_register >> 1);
         // Place new bit in MSB position
         next_rx_shift_register[DATA_W-1] = in_rx_in;
     }
      
     if (in_rx_parity && parity_enabled) {
         // Check parity if enabled
         bool expected_parity = calculate_parity(rx_shift_register);
         if (in_rx_in != expected_parity) {
             next_parity_error = true;
         }
     }
      
     if (in_rx_stop) {
         // Verify stop bit is 1
         if (in_rx_in != 1) {
             next_framing_error = true;
         }
         // Check for buffer overrun before storing
         else if (((rx_buf_head + 1) % RX_BUFFER_SIZE) == rx_buf_tail) {
             // Buffer full → flag overrun, don't store new byte
             next_overrun_error = true;
         }
         else if (!next_framing_error && !(parity_enabled && next_parity_error)) {
             // Only store data if no errors
             
             // Get the address for the RX buffer in Memory
             unsigned int mem_addr = RX_BUFFER_START + rx_buf_head;
             
             // Apply mask based on data_bits (if less than 8)
             sc_bv<8> masked_data = rx_shift_register;
             if (data_bits < 8) {
                 sc_bv<8> mask = (1 << data_bits) - 1;
                 masked_data &= mask;
             }
             
             // Set up data and address for writing to Memory
             out_addr = mem_addr;
             next_data_out = masked_data;  // This will be written to Memory
             
             // Signal to memory_map that we want to write to this address
             out_dp_write_enable = true;
             out_dp_addr = mem_addr;
             out_dp_data_in = masked_data;
             
             // Update head pointer
             next_rx_buf_head = (rx_buf_head + 1) % RX_BUFFER_SIZE;
         }
     }
     
     // CPU/host is reading from RX buffer
     if (in_rx_read && !out_rx_buffer_empty) {
         // Get the address for the current tail in Memory
         unsigned int mem_addr = RX_BUFFER_START + rx_buf_tail;
         
         // Read directly from Memory
         out_addr = mem_addr;
         next_data_out = in_data_in;  // data_in has the value from Memory
         
         // Update tail pointer
         next_rx_buf_tail = (rx_buf_tail + 1) % RX_BUFFER_SIZE;
     }
     
     // Recompute empty flag
     next_rx_buffer_empty = (next_rx_buf_head == next_rx_buf_tail);
 
     if (in_error_handle) {
         // Handle error conditions - clear error flags
         next_parity_error = false;
         next_framing_error = false;
         next_overrun_error = false;
     }
 }
 
 // Commit methods update actual registers based on next-state values
 void datapath::commit() {
     // TX updates
     out_tx_buffer_full = next_tx_buffer_full;
     out_tx_out = next_tx_out;
     tx_shift_register = next_tx_shift_register;
     
     // Update TX bit counter
     if (in_tx_data) {
         tx_bit_count++;
         if (tx_bit_count >= data_bits) {
             tx_bit_count = 0;
         }
     }
     
     // Reset bit counter when loading new data
     if (in_load_tx) {
         tx_bit_count = 0;
     }
     
     // RX updates
     out_rx_buffer_empty = next_rx_buffer_empty;
     out_parity_error = next_parity_error;
     out_framing_error = next_framing_error;
     out_overrun_error = next_overrun_error;
     rx_shift_register = next_rx_shift_register;
     rx_buf_head = next_rx_buf_head;
     rx_buf_tail = next_rx_buf_tail;
     out_data_out = next_data_out;
     
     // Update RX bit counter
     if (in_rx_data) {
         rx_bit_count++;
     }
     
     if (in_rx_start) {
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