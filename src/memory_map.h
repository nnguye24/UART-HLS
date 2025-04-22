#ifndef __MEMORY_MAP_H__
#define __MEMORY_MAP_H__

#include "systemc.h"
#include "sizes.h"
#include "stratus_hls.h" 

class memory_map {
    public:
    
    // Interface to external system
    sc_bv<DATA_W> data_in;
    sc_bv<DATA_W> data_out;
    sc_bv<ADDR_W> addr;
    bool read_write;
    bool write_enable;
    
    // Interface to datapath
    sc_bv<DATA_W> dp_data_out;
    sc_bv<DATA_W> dp_data_in;
    sc_bv<ADDR_W> dp_addr;
    bool dp_write_enable;
    
    // Status signals
    bool tx_buffer_full;
    bool rx_buffer_empty;
    bool error_indicator;
    
    // Memory array - single array for all memory
    sc_bv<DATA_W> Memory[RAM_SIZE];
    
    // Constructor
    memory_map();
    
    // Reset method
    void reset();
    
    // Declaration
    void compute();
    void commit();

    private:
    // Temporary values computed in compute()
    sc_bv<DATA_W> next_out_dout;
    sc_bv<4> next_out_io_out;
};

#endif