#include "datapath.h"
#include <iostream>

datapath::datapath() {
    reset();
  }

void datapath::reset() {
    // Add reset of our signals. 
}

void datapath::compute() {
    // Reset logic
    if (rst) {
        reset();
        return;
    }
    
    // Process TX and RX independently
    compute_tx();
    compute_rx();
}

// TX compute method
void datapath::compute_tx() {
    // Reset TX next-state values to defaults
    next_tx_buffer_full = false;
    next_tx_out = true; // Idle state is high
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
        // Send data bits
        next_tx_out = tx_shift_register[0];     // Access LSB 
        next_tx_shift_register = tx_shift_register >> 1; // Shift for next bit
    }
    
    if (tx_parity) {
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
    // Reset RX next-state values to defaults
    next_rx_buffer_empty = true;
    next_parity_error = false;
    next_framing_error = false;
    next_overrun_error = false;
    next_rx_shift_register = rx_shift_register;
    
    // Handle RX operations based on control signals
    if (rx_start) {
        // Verify start bit is 0
        if (rx_in != 0) {
            next_framing_error = true;
        }
    }
    
    if (rx_data) {
        // Receive data bit
        next_rx_shift_register = 
    }
    
    if (rx_parity) {
        // Check parity
        bool expected_parity = calculate_parity(rx_shift_register);
        if (rx_in != expected_parity) {
            next_parity_error = true;
        }
    }
    
    if (rx_stop) {
        // Verify stop bit is 1
        if (rx_in != 1) {
            next_framing_error = true;
        } else {
            // Store received data in buffer
            store_rx_register();
            
            // Check for buffer overrun
            // rx_buf_head is where the next byte will be stored
            // rx_buf_tail is where the next byte will be read
            //
            if (((rx_buf_head + 1) % RX_BUFFER_SIZE) == rx_buf_tail) {
                next_overrun_error = true;
            }
        }
    }
    
    if (error_handle) {
        // Handle error conditions, I don't really know what should be put here
        
    }
    
    // Update rx_buffer_empty status
    next_rx_buffer_empty = rx_buffer_check();
}

// Commit methods would update actual registers based on next-state values
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
}

void datapath::commit_rx() {
    // Update RX outputs and registers
    rx_buffer_empty = next_rx_buffer_empty;
    parity_error = next_parity_error;
    framing_error = next_framing_error;
    overrun_error = next_overrun_error;
    rx_shift_register = next_rx_shift_register;
    
    // Update RX bit counter
    if (rx_data) {
        rx_bit_count++;
    }
    
    if (rx_start) {
        rx_bit_count = 0;
    }
}