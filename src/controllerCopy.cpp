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
    state = IDLE;
    next_state = IDLE;
    
    // Reset all control outputs
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
    
    // Reset internal registers/counters
    bit_counter = 0;
    parity_value = false;
    tx_done = false;
    rx_done = false;
    parity_enabled = false;
    
    // Reset next-cycle values
    load_tx_next = false;
    tx_start_next = false;
    tx_data_next = false;
    tx_parity_next = false;
    tx_stop_next = false;
    rx_start_next = false;
    rx_data_next = false;
    rx_parity_next = false;
    rx_stop_next = false;
    error_handle_next = false;
    }
    
    void controller::compute() {
    // Reset all next-cycle outputs to default
    load_tx_next = false;
    tx_start_next = false;
    tx_data_next = false;
    tx_parity_next = false;
    tx_stop_next = false;
    rx_start_next = false;
    rx_data_next = false;
    rx_parity_next = false;
    rx_stop_next = false;
    error_handle_next = false;


    // Reset logic
    if (rst) {
        reset();
        return;
    }
    
    // State transition logic
    switch(state.to_uint()) {
        case IDLE:
        // Transition from IDLE state
        if (!tx_buffer_full) {
            next_state = LOAD_TX;
            load_tx_next = true;
        } else if (rx_in == 0) {
            next_state = RX_START_BIT;
            rx_start_next = true;
        } else {
            next_state = IDLE;
        }
        break;
        
        case LOAD_TX:
        // Transition from LOAD_TX state
        next_state = TX_START_BIT;
        tx_start_next = true;
        break;
        
        case TX_START_BIT:
        // Transition from TX_START_BIT state
        next_state = TX_DATA_BITS;
        tx_data_next = true;
        break;
        
        case TX_DATA_BITS:
        // Update bit counter in commit phase
        // Transition based on bit count
        tx_data_next = true;
        if (bit_counter >= 7) {  // Once we get above 7bits, we go next state
            // All 8 data bits sent
            if (parity_enabled) {
            next_state = TX_PARITY_BIT;
            } else {
            next_state = TX_STOP_BIT;
            }
        } else {
            next_state = TX_DATA_BITS;
        }
        break;
        
        case TX_PARITY_BIT:
        // Transition from TX_PARITY_BIT state
        tx_parity_next = true;
        next_state = TX_STOP_BIT;
        break;
        
        case TX_STOP_BIT:
        // Transition from TX_STOP_BIT state
        tx_stop_next = true;
        if (!tx_buffer_full) {
            next_state = LOAD_TX;  // More data to transmit
        } else {
            next_state = IDLE;     // Return to idle
        }
        break;
        
        case RX_WAIT:
        // Transition from RX_WAIT state
        if (rx_in == 0) {
            next_state = RX_START_BIT;
            rx_start_next = true;
        } else {
            next_state = RX_WAIT;
        }
        break;
        
        case RX_START_BIT:
        // Transition from RX_START_BIT state
        rx_start_next = true;
        next_state = RX_DATA_BITS;
        break;
        
        case RX_DATA_BITS:
        // Transition from RX_DATA_BITS state
        rx_data_next = true;
        if (bit_counter >= 7) {  // 8 bits (0-7)
            // All 8 data bits received
            if (parity_enabled) {
            next_state = RX_PARITY_CHECK;
            } else {
            next_state = RX_STOP_BIT;
            }
        } else {
            next_state = RX_DATA_BITS;
        }
        break;
        
        case RX_PARITY_CHECK:
        // Transition from RX_PARITY_CHECK state
        rx_parity_next = true;
        if (parity_error) {
            next_state = ERROR_HANDLING;
        } else {
            next_state = RX_STOP_BIT;
        }
        break;
        
        case RX_STOP_BIT:
        // Transition from RX_STOP_BIT state
        rx_stop_next = true;
        if (rx_in != 1) {
            // Framing error - no stop bit detected
            next_state = ERROR_HANDLING;
        } else {
            next_state = IDLE;
        }
        break;
        
        case ERROR_HANDLING:
        // Transition from ERROR_HANDLING state
        error_handle_next = true;
        next_state = IDLE;
        break;
        
        default:
        // Default case - return to IDLE
        next_state = IDLE;
        break;
    }
    }
    
    void controller::commit() {
    // Go to next state
    state = next_state;

    // Update outputs
    out_load_tx = load_tx_next;
    out_tx_start = tx_start_next;
    out_tx_data = tx_data_next;
    out_tx_parity = tx_parity_next;
    out_tx_stop = tx_stop_next;
    out_rx_start = rx_start_next;
    out_rx_data = rx_data_next;
    out_rx_parity = rx_parity_next;
    out_rx_stop = rx_stop_next;
    out_error_handle = error_handle_next;
    
    // Update internal counters/flags
    if (state == TX_DATA_BITS || state == RX_DATA_BITS) {
        bit_counter++;
    }
    
    if (state == LOAD_TX || state == RX_START_BIT) {
        bit_counter = 0;
    }
    
    if (state == TX_STOP_BIT) {
        tx_done = true;
    }
    
    if (state == RX_STOP_BIT && rx_in == 1) {
        rx_done = true;
    }
    }