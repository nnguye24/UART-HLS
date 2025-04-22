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

// Reset method to initialize memory map to default state
void memory_map::reset() {
    // Initialize config registers to default values
    for (int i = 0; i < CONFIG_REG_SIZE; i++) {
        config_reg[i].write(0);
    }
    
    // Initialize status registers
    for (int i = 0; i < STATUS_REG_SIZE; i++) {
        status_reg[i].write(0);
    }
    
    // Initialize buffers
    for (int i = 0; i < TX_BUFFER_SIZE; i++) {
        local_tx_buffer[i].write(0);
    }
    
    for (int i = 0; i < RX_BUFFER_SIZE; i++) {
        local_rx_buffer[i].write(0);
    }
    
    // Initialize outputs
    data_out.write(0);
    dp_data_out.write(0);
    dp_addr.write(0);
    dp_write_enable.write(false);
}

// Memory access implementation
void memory_map::memory_access() {
    // Reset logic
    if (rst.read()) {
        reset();
        return;
    }
    
    // Memory access logic
    if (chip_select.read()) {
        // Map address to appropriate memory region
        
        // Handle read operations
        if (!read_write.read()) {
            // Read operation
            // Address decoding logic
            // Set data_out based on address
        }
        // Handle write operations
        else if (read_write.read() && write_enable.read()) {
            // Write operation
            // Address decoding logic
            // Write data_in to appropriate location
        }
    }
}

// Status update implementation
void memory_map::status_update() {
    // Update status registers based on status signals
    
    // Line status register update
    sc_uint<DATA_W> line_status = status_reg[0].read();
    
    // Set bits based on status signals
    if (tx_buffer_full.read()) {
        line_status |= (1 << 6); // Bit 6 for TX buffer full
    } else {
        line_status &= ~(1 << 6);
    }
    
    if (rx_buffer_empty.read()) {
        line_status |= (1 << 0); // Bit 0 for RX buffer empty
    } else {
        line_status &= ~(1 << 0);
    }
    
    if (error_indicator.read()) {
        line_status |= (1 << 3); // Bit 3 for error indicator
    }
    
    status_reg[0].write(line_status);
}
