/**************************************************************
 * File Name: memory_map.cpp
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/22/2025
 *
 * Memory map implementation for UART controller
 **************************************************************/

 #include "memory_map.h"
 #include <iostream>
 
 using namespace std;
 
 void memory_map::reset() {
     // Initialize all memory to zero
     for (int i = 0; i < RAM_SIZE; ++i) {
         Memory[i] = 0;
     }
     
     // Initialize outputs
     data_out.write(0);
     dp_data_out.write(0);
     dp_addr.write(0);
     dp_write_enable.write(false);
 }
 
 void memory_map::memory_access() {
     // Reset logic
     if (rst.read()) {
         reset();
         return;
     }
     
     // Memory access logic when chip is selected
     if (chip_select.read()) {
         sc_uint<ADDR_W> selected_addr = addr.read();
         
         // Handle read operations
         if (!read_write.read()) {
             // Read operation
             if (selected_addr < RAM_SIZE) {
                 data_out.write(Memory[selected_addr]);
             }
         }
         // Handle write operations
         else if (write_enable.read()) {
             // Write operation
             if (selected_addr < RAM_SIZE) {
                 Memory[selected_addr] = data_in.read();
             }
         }
     }
     
     // Update datapath output
     dp_data_out.write(Memory[dp_addr.read()]);
 }
 
 void memory_map::status_update() {
     // Update memory status registers based on input signals
     
     // Status register at offset 38 (first status register)
     sc_bv<DATA_W> status = 0;
     
     // Set bits based on status signals
     if (tx_buffer_full.read()) {
         status[0] = 1; // Bit 0 for TX buffer full
     }
     
     if (rx_buffer_empty.read()) {
         status[1] = 1; // Bit 1 for RX buffer empty
     }
     
     if (error_indicator.read()) {
         status[2] = 1; // Bit 2 for error indicator
     }
     
     // Write to status register in memory (addr 38)
     Memory[38] = status;
 }