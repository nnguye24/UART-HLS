#ifndef __MEMORY_MAP_H__
#define __MEMORY_MAP_H__

#include "systemc.h"
#include "sizes.h"

class memory_map {
public:
    bool rst;
    <sc_uint<DATA_W>> data_in;
    <sc_uint<DATA_W>> data_out;
    <sc_uint<ADDR_W>> addr;
    bool chip_select;
    bool read_write;
    bool write_enable;
    
    // Interface to datapath
    <sc_uint<DATA_W>> dp_data_out;
    <sc_uint<DATA_W>> dp_data_in;
    <sc_uint<ADDR_W>> dp_addr;
    bool dp_write_enable;
    
    // Status signals
<<<<<<< HEAD
    sc_in<bool> tx_buffer_full; //Transmit Data
    sc_in<bool> rx_buffer_empty;//Received
    sc_in<bool> error_indicator;
=======
    bool tx_buffer_full;
    bool rx_buffer_empty;
    bool error_indicator;
>>>>>>> 32cec010363f71286bb12aba5dad602301e17bbd
    
    // Memory array - single array for all memory
    sc_uint<DATA_W> Memory[RAM_SIZE];
    
    // Constructor
    memory_map();
    
    // Reset method
    void reset();
    
    // Main memory access method
    void memory_access();
    
    // Update status registers based on UART state
    void status_update();
    
    // Handle register-specific updates
    void handle_register_change(sc_uint<ADDR_W> reg_addr, sc_uint<DATA_W> value);
    
    // Helper methods for accessing specific memory regions
    sc_uint<DATA_W> get_tx_buffer(unsigned int index);
    void set_tx_buffer(unsigned int index, sc_uint<DATA_W> value);
    sc_uint<DATA_W> get_rx_buffer(unsigned int index);
    void set_rx_buffer(unsigned int index, sc_uint<DATA_W> value);
    
    // Configuration access methods
    sc_uint<DATA_W> get_baud_rate_divisor();
    bool get_parity_enabled();
    bool get_parity_even();
    unsigned int get_data_bits();
    unsigned int get_stop_bits();
    
    // Standard compute/commit methods for HLS pattern
    void compute();
    void commit();

private:
    // Temporary values computed in compute()
    sc_uint<DATA_W> next_out_dout;
    sc_uint<4> next_out_io_out;
};

#endif
