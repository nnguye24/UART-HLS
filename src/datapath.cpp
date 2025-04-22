/**************************************************************
 * File Name: datapath.cpp
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/22/2025
 *
 * UART Datapath implementation
 **************************************************************/

#include "datapath.h"
#include <iostream>

using namespace std;

// Reset method to initialize datapath to default state
void datapath::reset() {
    tx_shift_reg.write(0);
    rx_shift_reg.write(0);
    tx_holding_reg.write(0);
    rx_holding_reg.write(0);
    tx_buffer_count.write(0);
    rx_buffer_count.write(0);
    
    // Initialize buffers and registers
    for (int i = 0; i < TX_BUFFER_SIZE; i++) {
        tx_buffer[i].write(0);
    }
    
    for (int i = 0; i < RX_BUFFER_SIZE; i++) {
        rx_buffer[i].write(0);
    }
    
    for (int i = 0; i < CONFIG_REG_SIZE; i++) {
        config_reg[i].write(0);
    }
    
    for (int i = 0; i < STATUS_REG_SIZE; i++) {
        status_reg[i].write(0);
    }
    
    // Set initial output values
    tx_buffer_full.write(false);
    rx_buffer_empty.write(true);
    parity_error.write(false);
    framing_error.write(false);
    overrun_error.write(false);
    tx_out.write(1);  // Idle line is high
    data_out.write(0);
}

// Transmit logic implementation
void datapath::transmit_logic() {
    // Reset logic
    if (rst.read()) {
        reset();
        return;
    }
    
    // Transmit state machine logic
    if (load_tx.read()) {
        // Load data from buffer to shift register
    }
    
    if (tx_start.read()) {
        // Transmit start bit (logic low)
        tx_out.write(0);
    }
    
    if (tx_data.read()) {
        // Transmit data bits serially
    }
    
    if (tx_parity.read()) {
        // Calculate and transmit parity bit
    }
    
    if (tx_stop.read()) {
        // Transmit stop bit (logic high)
        tx_out.write(1);
    }
}

// Receive logic implementation
void datapath::receive_logic() {
    // Reset logic
    if (rst.read()) {
        reset();
        return;
    }
    
    // Receive state machine logic
    if (rx_start.read()) {
        // Sample start bit and synchronize
    }
    
    if (rx_data.read()) {
        // Receive data bits serially
    }
    
    if (rx_parity.read()) {
        // Check parity bit
    }
    
    if (rx_stop.read()) {
        // Verify stop bit
    }
}

// Buffer management implementation
void datapath::buffer_management() {
    // Reset logic
    if (rst.read()) {
        reset();
        return;
    }
    
    // Handle buffer operations based on memory address and control signals
    
    // Update buffer counts based on operations
}

// Status update implementation
void datapath::status_update() {
    // Update tx_buffer_full status
    if (tx_buffer_count.read() >= TX_BUFFER_SIZE) {
        tx_buffer_full.write(true);
    } else {
        tx_buffer_full.write(false);
    }
    
    // Update rx_buffer_empty status
    if (rx_buffer_count.read() == 0) {
        rx_buffer_empty.write(true);
    } else {
        rx_buffer_empty.write(false);
    }
}
