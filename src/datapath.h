#ifndef __DATAPATH_H__
#define __DATAPATH_H__

#include "systemc.h"
#include "sizes.h"
#include "stratus_hls.h" // Cadence Stratus

SC_MODULE(datapath) {
    // Clock and reset
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    // Control signals from controller
    sc_in<bool> load_tx;
    sc_in<bool> tx_start;
    sc_in<bool> tx_data;
    sc_in<bool> tx_parity;
    sc_in<bool> tx_stop;
    sc_in<bool> rx_start;
    sc_in<bool> rx_data;
    sc_in<bool> rx_parity;
    sc_in<bool> rx_stop;
    sc_in<bool> error_handle;
    
    // Status signals to controller
    sc_out<bool> tx_buffer_full;
    sc_out<bool> rx_buffer_empty;
    sc_out<bool> parity_error;
    sc_out<bool> framing_error;
    sc_out<bool> overrun_error;
    
    // UART external signals
    sc_in<bool> rx_in;    // RXD serial input
    sc_out<bool> tx_out;  // TXD serial output
    
    // Data and address buses
    sc_in<sc_uint<DATA_W>> data_in;
    sc_out<sc_uint<DATA_W>> data_out;
    sc_in<sc_uint<ADDR_W>> addr;
    
    // Internal registers and buffers
    sc_signal<sc_uint<DATA_W>> tx_shift_reg;
    sc_signal<sc_uint<DATA_W>> rx_shift_reg;
    sc_signal<sc_uint<DATA_W>> tx_holding_reg;
    sc_signal<sc_uint<DATA_W>> rx_holding_reg;
    
    // Buffer management
    sc_signal<int> tx_buffer_count;
    sc_signal<int> rx_buffer_count;
    
    // RAM arrays
    sc_signal<sc_uint<DATA_W>> tx_buffer[TX_BUFFER_SIZE];
    sc_signal<sc_uint<DATA_W>> rx_buffer[RX_BUFFER_SIZE];
    sc_signal<sc_uint<DATA_W>> config_reg[CONFIG_REG_SIZE];
    sc_signal<sc_uint<DATA_W>> status_reg[STATUS_REG_SIZE];
    
    // Constructor
    SC_CTOR(datapath) {
        // Register processes
        SC_METHOD(transmit_logic);
        sensitive << clk.pos() << rst;
        
        SC_METHOD(receive_logic);
        sensitive << clk.pos() << rst << rx_in;
        
        SC_METHOD(buffer_management);
        sensitive << clk.pos() << rst;
        
        SC_METHOD(status_update);
        sensitive << tx_buffer_count << rx_buffer_count;
    }
    
    // Reset method
    void reset();
    
    // Declarations for processes
    void transmit_logic();
    void receive_logic();
    void buffer_management();
    void status_update();
};

#endif
