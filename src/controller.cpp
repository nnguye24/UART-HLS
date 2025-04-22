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

// Reset method to initialize controller to default state
void controller::reset() {
    state.write(IDLE);
}

// Next state logic implementation
void controller::next_state_logic() {
    // Reset logic
    if (rst.read()) {
        reset();
        return;
    }
    
    // State transition logic
    switch(state.read()) {
        case IDLE:
            // Transition logic from IDLE state
            break;
            
        case LOAD_TX:
            // Transition logic from LOAD_TX state
            break;
            
        case TX_START_BIT:
            // Transition logic from TX_START_BIT state
            break;
            
        case TX_DATA_BITS:
            // Transition logic from TX_DATA_BITS state
            break;
            
        case TX_PARITY_BIT:
            // Transition logic from TX_PARITY_BIT state
            break;
            
        case TX_STOP_BIT:
            // Transition logic from TX_STOP_BIT state
            break;
            
        case RX_WAIT:
            // Transition logic from RX_WAIT state
            break;
            
        case RX_START_BIT:
            // Transition logic from RX_START_BIT state
            break;
            
        case RX_DATA_BITS:
            // Transition logic from RX_DATA_BITS state
            break;
            
        case RX_PARITY_CHECK:
            // Transition logic from RX_PARITY_CHECK state
            break;
            
        case RX_STOP_BIT:
            // Transition logic from RX_STOP_BIT state
            break;
            
        case ERROR_HANDLING:
            // Transition logic from ERROR_HANDLING state
            break;
            
        default:
            // Default case - return to IDLE
            state.write(IDLE);
            break;
    }
}

// Output logic implementation
void controller::output_logic() {
    // Default values for all outputs
    load_tx.write(false);
    tx_start.write(false);
    tx_data.write(false);
    tx_parity.write(false);
    tx_stop.write(false);
    rx_start.write(false);
    rx_data.write(false);
    rx_parity.write(false);
    rx_stop.write(false);
    error_handle.write(false);
    
    // Output logic based on current state
    switch(state.read()) {
        case IDLE:
            // Output signals for IDLE state
            break;
            
        case LOAD_TX:
            // Output signals for LOAD_TX state
            load_tx.write(true);
            break;
            
        case TX_START_BIT:
            // Output signals for TX_START_BIT state
            tx_start.write(true);
            break;
            
        case TX_DATA_BITS:
            // Output signals for TX_DATA_BITS state
            tx_data.write(true);
            break;
            
        case TX_PARITY_BIT:
            // Output signals for TX_PARITY_BIT state
            tx_parity.write(true);
            break;
            
        case TX_STOP_BIT:
            // Output signals for TX_STOP_BIT state
            tx_stop.write(true);
            break;
            
        case RX_WAIT:
            // Output signals for RX_WAIT state
            break;
            
        case RX_START_BIT:
            // Output signals for RX_START_BIT state
            rx_start.write(true);
            break;
            
        case RX_DATA_BITS:
            // Output signals for RX_DATA_BITS state
            rx_data.write(true);
            break;
            
        case RX_PARITY_CHECK:
            // Output signals for RX_PARITY_CHECK state
            rx_parity.write(true);
            break;
            
        case RX_STOP_BIT:
            // Output signals for RX_STOP_BIT state
            rx_stop.write(true);
            break;
            
        case ERROR_HANDLING:
            // Output signals for ERROR_HANDLING state
            error_handle.write(true);
            break;
            
        default:
            // Default case - no control signals active
            break;
    }
}
