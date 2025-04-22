#ifndef __MEMORY_MAP_H__
#define __MEMORY_MAP_H__

#include "systemc.h"
#include "sizes.h"
#include "stratus_hls.h" // Cadence Stratus

SC_MODULE(memory_map) {
    // Clock and reset
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    // Interface to external system
    sc_in<sc_uint<DATA_W>> data_in;
    sc_out<sc_uint<DATA_W>> data_out;
    sc_in<sc_uint<ADDR_W>> addr;
    sc_in<bool> chip_select;
    sc_in<bool> read_write;
    sc_in<bool> write_enable;
    
    // Interface to datapath
    sc_out<sc_uint<DATA_W>> dp_data_out;
    sc_in<sc_uint<DATA_W>> dp_data_in;
    sc_out<sc_uint<ADDR_W>> dp_addr;
    sc_out<bool> dp_write_enable;
    
    // Status signals
    sc_in<bool> tx_buffer_full;
    sc_in<bool> rx_buffer_empty;
    sc_in<bool> error_indicator;
    
    // Memory array - single array for all memory
    sc_bv<DATA_W> Memory[RAM_SIZE];
    
    // Constructor
    SC_CTOR(memory_map) {
        // Register processes
        SC_METHOD(memory_access);
        sensitive << clk.pos() << rst;
        
        SC_METHOD(status_update);
        sensitive << tx_buffer_full << rx_buffer_empty << error_indicator;
    }
    
    // Reset method
    void reset();
    
    // Declarations for processes
    void memory_access();
    void status_update();
};

#endif