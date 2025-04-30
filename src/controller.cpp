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

void controller::process() {
    {
        HLS_DEFINE_PROTOCOL("reset");
        reset_control_clear_regs();
    }
    
    {       
        HLS_DEFINE_PROTOCOL("wait");
        wait();
    }

    while(true) {
        // Read inputs
        {
            HLS_DEFINE_PROTOCOL("input");
            read_inputs();
        }
        
        // Check if memory write is active
        if(!in_mem_we) {
            if(in_start) {
                {
                    HLS_DEFINE_PROTOCOL("start");
                    reset_control_clear_regs();
                }
            } else {
                {
                    HLS_DEFINE_PROTOCOL("controller_fsm");
                    controller_fsm();
                }
                
                {
                    HLS_DEFINE_PROTOCOL("output");
                    write_outputs();
                }
            }
        }
        
        // Wait for next cycle
        {       
            HLS_DEFINE_PROTOCOL("wait");
            wait();
        }

        // Additional wait for processing time
        {       
            HLS_DEFINE_PROTOCOL("wait");
            wait();
        }
    }
}

void controller::reset_control_clear_regs() {
    // Reset state registers
    tx_state = TX_IDLE;
    tx_next_state = TX_IDLE;
    rx_state = RX_IDLE;
    rx_next_state = RX_IDLE;
    
    // Reset counters and flags
    tx_bit_counter = 0;
    rx_bit_counter = 0;
    tx_parity_value = false;
    rx_parity_value = false;
    tx_done = false;
    rx_done = false;
    
    // Clear all outputs
    clear_output_sc_bits();
}

void controller::clear_output_sc_bits() {
    // Clear all control outputs
    out_load_tx = false;
    out_load_tx2 = false;
    out_tx_start = false;
    out_tx_data = false;
    out_tx_parity = false;
    out_tx_stop = false;
    out_rx_start = false;
    out_rx_data = false;
    out_rx_parity = false;
    out_rx_stop = false;
    out_rx_read = false;
    out_error_handle = false;
}

void controller::read_inputs() {
    // Read all input ports
    in_start = start.read();
    in_mem_we = mem_we.read();
    in_tx_buffer_full = tx_buffer_full.read();
    in_rx_buffer_empty = rx_buffer_empty.read();
    in_rx_in = rx_in.read();
    in_parity_error = parity_error.read();
    in_framing_error = framing_error.read();
    in_overrun_error = overrun_error.read();
    
    // Read configuration
    in_parity_enabled = parity_enabled.read();
    in_parity_even = parity_even.read();
    in_data_bits = data_bits.read();
    in_stop_bits = stop_bits.read();
}

void controller::controller_fsm() {
    // Skip if memory write is active
    if(in_mem_we) {
        return;
    }
    else if(in_start) {
        // Reset on start signal
        tx_state = TX_IDLE;
        tx_next_state = TX_IDLE;
        rx_state = RX_IDLE;
        rx_next_state = RX_IDLE;
        in_start = false;
        return;
    }
    else {
        // Normal FSM operation
        // First, clear all outputs from previous cycles
        clear_output_sc_bits();
        
        // Update current state from next state
        tx_state = tx_next_state;
        rx_state = rx_next_state;
        
        // TX FSM logic
        switch(tx_state.to_uint()) {
            case TX_IDLE:
                if(!in_tx_buffer_full) {
                    out_load_tx = true;
                    tx_next_state = LOAD_TX2;
                } else {
                    tx_next_state = TX_IDLE;
                }
                break;
                
            case LOAD_TX2:
                out_load_tx2 = true;
                tx_next_state = TX_START_BIT;
                break;
                
            case TX_START_BIT:
                out_tx_start = true;
                tx_next_state = TX_DATA_BITS;
                tx_bit_counter = 0;
                break;
                
            case TX_DATA_BITS:
                out_tx_data = true;
                
                if(tx_bit_counter >= in_data_bits - 1) {
                    if(in_parity_enabled) {
                        tx_next_state = TX_PARITY_BIT;
                    } else {
                        tx_next_state = TX_STOP_BIT;
                    }
                } else {
                    tx_next_state = TX_DATA_BITS;
                    tx_bit_counter++;
                }
                break;
                
            case TX_PARITY_BIT:
                out_tx_parity = true;
                tx_next_state = TX_STOP_BIT;
                break;
                
            case TX_STOP_BIT:
                out_tx_stop = true;
                
                if(in_stop_bits == 2 && tx_bit_counter == 0) {
                    tx_bit_counter = 1;
                    tx_next_state = TX_STOP_BIT;
                } else {
                    if(!in_tx_buffer_full) {
                        tx_next_state = TX_IDLE;
                        tx_done = true;
                    } else {
                        tx_next_state = TX_IDLE;
                    }
                }
                break;
                
            default:
                tx_next_state = TX_IDLE;
                break;
        }
        
        // RX FSM logic
        switch(rx_state.to_uint()) {
            case RX_IDLE:
                if(in_rx_in == true) {
                    out_rx_start = true;
                    rx_next_state = RX_START_BIT;
                } else {
                    rx_next_state = RX_IDLE;
                }
                break;
                
            case RX_START_BIT:
                out_rx_start = true;
                
                if(!in_rx_in) {
                    rx_next_state = RX_IDLE;
                } else {
                    rx_next_state = RX_DATA_BITS;
                    rx_bit_counter = 0;
                }
                break;
                
            case RX_DATA_BITS:
                out_rx_data = true;
                
                if(rx_bit_counter >= in_data_bits - 1) {
                    if(in_parity_enabled) {
                        rx_next_state = RX_PARITY_CHECK;
                    } else {
                        rx_next_state = RX_STOP_BIT;
                    }
                } else {
                    rx_next_state = RX_DATA_BITS;
                    rx_bit_counter++;
                }
                break;
                
            case RX_PARITY_CHECK:
                out_rx_parity = true;
                
                if(in_parity_error) {
                    rx_next_state = ERROR_HANDLING;
                } else {
                    rx_next_state = RX_STOP_BIT;
                }
                break;
                
            case RX_STOP_BIT:
                out_rx_stop = true;
                
                if(in_rx_in != 1) {
                    rx_next_state = ERROR_HANDLING;
                } else {
                    if(in_stop_bits == 2 && rx_bit_counter == 0) {
                        rx_bit_counter = 1;
                        rx_next_state = RX_STOP_BIT;
                    } else {
                        rx_next_state = RX_IDLE;
                        out_rx_read = true;
                        rx_done = true;
                    }
                }
                break;
                
            case ERROR_HANDLING:
                out_error_handle = true;
                rx_next_state = RX_IDLE;
                break;
                
            default:
                rx_next_state = RX_IDLE;
                break;
        }
    }
}

void controller::write_outputs() {
    // Write to all output ports
    load_tx.write(out_load_tx);
    load_tx2.write(out_load_tx2);
    tx_start.write(out_tx_start);
    tx_data.write(out_tx_data);
    tx_parity.write(out_tx_parity);
    tx_stop.write(out_tx_stop);
    rx_start.write(out_rx_start);
    rx_data.write(out_rx_data);
    rx_parity.write(out_rx_parity);
    rx_stop.write(out_rx_stop);
    rx_read.write(out_rx_read);
    error_handle.write(out_error_handle);
}

bool controller::test_reset_controller() {
    // Check all state registers are reset
    if(tx_state != TX_IDLE) {
        cout << "tx_state not reset: " << tx_state << endl;
        return false;
    }
    
    if(rx_state != RX_IDLE) {
        cout << "rx_state not reset: " << rx_state << endl;
        return false;
    }
    
    if(tx_next_state != TX_IDLE) {
        cout << "tx_next_state not reset: " << tx_next_state << endl;
        return false;
    }
    
    if(rx_next_state != RX_IDLE) {
        cout << "rx_next_state not reset: " << rx_next_state << endl;
        return false;
    }
    if(tx_buffer_full){
        std::cout << "TX_BUFFER_NOT RESET" << std::endl;
    }
    if(!rx_buffer_empty){
        std::cout < "RX_BUFFER_NOT_RESET" << std::endl;
    }
    
    // Check all outputs are reset
    if(out_load_tx) return false;
    if(out_load_tx2) return false;
    if(out_tx_start) return false;
    if(out_tx_data) return false;
    if(out_tx_parity) return false;
    if(out_tx_stop) return false;
    if(out_rx_start) return false;
    if(out_rx_data) return false;
    if(out_rx_parity) return false;
    if(out_rx_stop) return false;
    if(out_rx_read) return false;
    if(out_error_handle) return false;
    
    return true;
}

