#ifndef __TOP_H__
#define __TOP_H__

#include "systemc.h"
#include "stratus_hls.h"
#include "sizes.h"
#include "datapath.h"
#include "controller.h"
#include "memory_map.h"

SC_MODULE(top) {
  // Inputs from testbench
  sc_in<bool> clk;
  sc_in<bool> rst;
  sc_in<sc_uint<DATA_W>> data_in;
  sc_in<sc_uint<ADDR_W>> addr;
  sc_in<bool> chip_select;
  sc_in<bool> read_write;
  sc_in<bool> write_enable;
  sc_in<bool> rx_in;

  // Outputs to testbench
  sc_out<sc_uint<DATA_W>> data_out;
  sc_out<bool> tx_out;
  sc_out<bool> tx_buffer_full;
  sc_out<bool> rx_buffer_empty;
  sc_out<bool> error_indicator;

  // Submodules
  datapath datapath_inst;
  controller controller_inst;
  memory_map memory_map_inst;

  // Top-level thread
  void process();
  void compute();
  void commit();

  SC_CTOR(top) : datapath_inst(), controller_inst(), memory_map_inst() {
    SC_THREAD(process);
    sensitive << clk.pos();
    async_reset_signal_is(rst, false);
  }
  
#ifdef NC_SYSTEMC
 public:
  void ncsc_replace_names(){
    // Replace all the names using ncsc_replace_name for inputs and outputs
    
    // Inputs
    ncsc_replace_name(clk, "clk");                      // Port 0
    ncsc_replace_name(rst, "rst");                      // Port 1
    ncsc_replace_name(data_in, "data_in");              // Port 2
    ncsc_replace_name(addr, "addr");                    // Port 3
    ncsc_replace_name(chip_select, "chip_select");      // Port 4
    ncsc_replace_name(read_write, "read_write");        // Port 5
    ncsc_replace_name(write_enable, "write_enable");    // Port 6
    ncsc_replace_name(rx_in, "rx_in");                  // Port 7
    
    // Outputs
    ncsc_replace_name(data_out, "data_out");            // Port 8
    ncsc_replace_name(tx_out, "tx_out");                // Port 9
    ncsc_replace_name(tx_buffer_full, "tx_buffer_full");// Port 10
    ncsc_replace_name(rx_buffer_empty, "rx_buffer_empty");// Port 11
    ncsc_replace_name(error_indicator, "error_indicator");// Port 12
  }
#endif
};

#endif