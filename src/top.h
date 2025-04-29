// uart_top.h

#ifndef __UART_TOP_H__
#define __UART_TOP_H__

#include "systemc.h"
#include "stratus_hls.h"
#include "sizes.h"
#include "datapath.h"
#include "controller.h"
#include "memory_map.h"

SC_MODULE(top) {
  // Inputs from testbench
  sc_in<bool> clk;
  sc_in<bool> reset;
  sc_in<bool> tx_start;
  sc_in<sc_bv<DATA_W>> tx_data;
  sc_in<bool> rx_in;

  // Outputs to testbench
  sc_out<bool> tx_out;
  sc_out<bool> tx_busy;
  sc_out<sc_bv<DATA_W>> rx_data;
  sc_out<bool> rx_ready;

  // Submodules
  datapath datapath_inst;
  controller controller_inst;
  memory_map memory_map_inst;

  // Top-level thread
  void process();
  void compute();
  void commit();

  SC_CTOR(uart_top) : tx_inst(), rx_inst(), controller_inst() {
    SC_THREAD(process);
    sensitive << clk.pos();
    async_reset_signal_is(reset, false);
  }
  
#ifdef NC_SYSTEMC
 public:
  void ncsc_replace_names(){
    // Replace all the names using ncsc_replace_name for inputs and outputs
    
    // Inputs
    ncsc_replace_name(clk, "clk");                  // Port 0
    ncsc_replace_name(reset, "reset");              // Port 1
    ncsc_replace_name(tx_start, "tx_start");        // Port 2
    ncsc_replace_name(tx_data, "tx_data");          // Port 3
    ncsc_replace_name(rx_in, "rx_in");              // Port 4
    
    // Outputs
    ncsc_replace_name(tx_out, "tx_out");            // Port 5
    ncsc_replace_name(tx_busy, "tx_busy");          // Port 6
    ncsc_replace_name(rx_data, "rx_data");          // Port 7
    ncsc_replace_name(rx_ready, "rx_ready");        // Port 8
  }
#endif
};

#endif