#ifndef __TOP_H__
#define __TOP_H__

#include "systemc.h"
#include "controller.h"
#include "datapath.h"
#include "memory_map.h"
#include "sizes.h"
#include "stratus_hls.h" // Cadence Stratus

SC_MODULE(top) {
    // Clock and reset
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    // External system interface
    sc_in<sc_uint<DATA_W>> data_in;
    sc_out<sc_uint<DATA_W>> data_out;
    sc_in<sc_uint<ADDR_W>> addr;
    sc_in<bool> chip_select;
    sc_in<bool> read_write;
    sc_in<bool> write_enable;
    
    // UART serial interface
    sc_in<bool> rx_in;    // RXD serial input
    sc_out<bool> tx_out;  // TXD serial output
    
    // Status outputs
    sc_out<bool> tx_buffer_full;
    sc_out<bool> rx_buffer_empty;
    sc_out<bool> error_indicator;
    
    // Internal components
    controller* ctrl;
    datapath* dp;
    memory_map* mem;
    
    // Internal signals
    sc_signal<bool> load_tx;
    sc_signal<bool> tx_start;
    sc_signal<bool> tx_data;
    sc_signal<bool> tx_parity;
    sc_signal<bool> tx_stop;
    sc_signal<bool> rx_start;
    sc_signal<bool> rx_data;
    sc_signal<bool> rx_parity;
    sc_signal<bool> rx_stop;
    sc_signal<bool> error_handle;
    
    sc_signal<bool> parity_error;
    sc_signal<bool> framing_error;
    sc_signal<bool> overrun_error;
    
    sc_signal<sc_uint<DATA_W>> dp_data_out;
    sc_signal<sc_uint<DATA_W>> dp_data_in;
    sc_signal<sc_uint<ADDR_W>> dp_addr;
    sc_signal<bool> dp_write_enable;
    
    // Constructor
    SC_CTOR(top) {
        // Instantiate controller
        ctrl = new controller("controller_inst");
        ctrl->clk(clk);
        ctrl->rst(rst);
        ctrl->load_tx(load_tx);
        ctrl->tx_start(tx_start);
        ctrl->tx_data(tx_data);
        ctrl->tx_parity(tx_parity);
        ctrl->tx_stop(tx_stop);
        ctrl->rx_start(rx_start);
        ctrl->rx_data(rx_data);
        ctrl->rx_parity(rx_parity);
        ctrl->rx_stop(rx_stop);
        ctrl->error_handle(error_handle);
        ctrl->tx_buffer_full(tx_buffer_full);
        ctrl->rx_buffer_empty(rx_buffer_empty);
        ctrl->parity_error(parity_error);
        ctrl->framing_error(framing_error);
        ctrl->overrun_error(overrun_error);
        
        // Instantiate datapath
        dp = new datapath("datapath_inst");
        dp->clk(clk);
        dp->rst(rst);
        dp->load_tx(load_tx);
        dp->tx_start(tx_start);
        dp->tx_data(tx_data);
        dp->tx_parity(tx_parity);
        dp->tx_stop(tx_stop);
        dp->rx_start(rx_start);
        dp->rx_data(rx_data);
        dp->rx_parity(rx_parity);
        dp->rx_stop(rx_stop);
        dp->error_handle(error_handle);
        dp->tx_buffer_full(tx_buffer_full);
        dp->rx_buffer_empty(rx_buffer_empty);
        dp->parity_error(parity_error);
        dp->framing_error(framing_error);
        dp->overrun_error(overrun_error);
        dp->rx_in(rx_in);
        dp->tx_out(tx_out);
        dp->data_in(dp_data_out);
        dp->data_out(dp_data_in);
        dp->addr(dp_addr);
        
        // Instantiate memory map
        mem = new memory_map("memory_map_inst");
        mem->clk(clk);
        mem->rst(rst);
        mem->data_in(data_in);
        mem->data_out(data_out);
        mem->addr(addr);
        mem->chip_select(chip_select);
        mem->read_write(read_write);
        mem->write_enable(write_enable);
        mem->dp_data_out(dp_data_out);
        mem->dp_data_in(dp_data_in);
        mem->dp_addr(dp_addr);
        mem->dp_write_enable(dp_write_enable);
        mem->tx_buffer_full(tx_buffer_full);
        mem->rx_buffer_empty(rx_buffer_empty);
        mem->error_indicator(error_indicator);
    }
    
    // Destructor to clean up dynamically allocated modules
    ~top() {
        delete ctrl;
        delete dp;
        delete mem;
    }
};

#endif
