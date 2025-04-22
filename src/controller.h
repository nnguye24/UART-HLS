#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "systemc.h"
#include "sizes.h"

SC_MODULE(controller) {
    // Clock and reset
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    // FSM state
    sc_signal<int> state;
    
    // Control signals to datapath
    sc_out<bool> load_tx;
    sc_out<bool> tx_start;
    sc_out<bool> tx_data;
    sc_out<bool> tx_parity;
    sc_out<bool> tx_stop;
    sc_out<bool> rx_start;
    sc_out<bool> rx_data;
    sc_out<bool> rx_parity;
    sc_out<bool> rx_stop;
    sc_out<bool> error_handle;
    
    // Status signals from datapath
    sc_in<bool> tx_buffer_full;
    sc_in<bool> rx_buffer_empty;
    sc_in<bool> parity_error;
    sc_in<bool> framing_error;
    sc_in<bool> overrun_error;
    
    // Constructor
    SC_CTOR(controller) {
        // Register processes
        SC_METHOD(next_state_logic);
        sensitive << clk.pos() << rst;
        
        SC_METHOD(output_logic);
        sensitive << state;
    }
    
    // Reset method
    void reset();
    
    // Declarations for processes
    void next_state_logic();
    void output_logic();
};

#endif