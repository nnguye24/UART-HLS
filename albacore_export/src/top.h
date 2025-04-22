

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
  sc_in<bool> reset;
  sc_in<bool> start;
  sc_in< sc_bv<ADDR_W> > mem_addr;
  sc_in< sc_bv<DATA_W> > mem_data;
  sc_in<bool> mem_we;
  sc_in< sc_bv<4> > io_in;

  // Outputs to testbench
  sc_out< sc_bv<DATA_W> > mem_data_out;
  sc_out< sc_bv<4> > io_out;

  // Submodules (now as plain classes)
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
    async_reset_signal_is(reset, false);
  }
 
  
#ifdef NC_SYSTEMC
 public:
  void ncsc_replace_names(){

    // Replace all the names using ncsc_replace_name for inputs and output

    // Inputs
    ncsc_replace_name(clk, "clk");                      // Port 0
    ncsc_replace_name(reset, "reset");                  // Port 1
    ncsc_replace_name(start, "start");                  // Port 2
    ncsc_replace_name(mem_addr, "mem_addr");            // Port 3
    ncsc_replace_name(mem_data, "mem_data");            // Port 4
    ncsc_replace_name(mem_we, "mem_we");                // Port 5
    ncsc_replace_name(io_in, "io_in");                  // Port 6
 
    ncsc_replace_name(mem_data_out, "mem_data_out");    // Port 7
    ncsc_replace_name(io_out, "io_out");                // Port 8

  }

#endif

};

#endif
