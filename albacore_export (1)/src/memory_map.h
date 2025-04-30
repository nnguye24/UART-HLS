/**************************************************************
 * File Name: memory_map.h
 * Author: Matthew Morrison
 * Date: 4/1/2025
 *
 * Contains the SystemC Module Header for the albacore Memory
 **************************************************************/

// Step 1a - Define the Header File
#ifndef __MEMORY_MAP_H__
#define __MEMORY_MAP_H__

// Step 2 - Include the libraries
#include "systemc.h" // SystemC Library
#include "stratus_hls.h" // Cadence Stratus

// Step 3 - Call sizes.h
#include "sizes.h"

// Step 4 Defining the SC_MODULE
SC_MODULE(memory_map){

  // Step 4-1 - Define the inputs and outputs to the datapath
  sc_in<bool> clk;                        // Port 0
  sc_in<bool> reset;                      // Port 1
  sc_in< sc_bv<ADDR_W> > mem_addr;
  sc_in< sc_bv<DATA_W> > mem_din;
  sc_in<bool> mem_we;
  sc_in< sc_bv<ADDR_W> > addr;            // Port 2
  sc_in< sc_bv<DATA_W> > din;             // Port 3
  sc_in<bool> we;                         // Port 4
  sc_in< sc_bv<4> > io_in;                // Port 5
  
  sc_out< sc_bv<DATA_W> > dout;           // Port 6
  sc_out< sc_bv<4> > io_out;              // Port 7
  
  // Step 5 - Create all the internal registers
  sc_bv<DATA_W> Memory[RAM_SIZE];
  sc_bv<4> q_io_out;
  sc_bv<4> q_io_in;
  sc_bv<DATA_W> dout_ram;
  
  sc_bit we_ram;
  sc_bit we_io_out;
  
  // Step 6-4 - Create a method called process to run the thread
  void process();
  
  // Read the inputs
  void read_inputs();

  // The unique functions you define
  void map_memory();

  // Reset the memory
  void reset_memory_map();

  bool test_reset_memory_map();

  // Write the outputs
  void write_outputs();

    // Step 8-2 - Create the intermediate values from the inputs/outputs from the SC_MODULE - No clk or reset
    // Replace any bool with sc_bit
  sc_bv<ADDR_W> in_mem_addr;
  sc_bv<DATA_W> in_mem_din;
  sc_bit in_mem_we;
  sc_bv<ADDR_W> in_addr;            // Port 2
  sc_bv<DATA_W> in_din;             // Port 3
  sc_bit in_we;                     // Port 4
  sc_bv<4> in_io_in;                // Port 5
  
  sc_bv<DATA_W> out_dout;           // Port 6
  sc_bv<4> out_io_out;              // Port 7
  
  // Step 6-1 - Define the Constructor
  SC_CTOR(memory_map){
    // Step 6-2 - Define the Clocked thread
    SC_THREAD(process);
    
    // Step 6-3 - Define the Asynchronous reset
    sensitive << clk.pos();

    async_reset_signal_is(reset, false);
  }
  
  
  // Step 4-2 - Implement NC_SYSTEMC
#ifdef NC_SYSTEMC
 public:
   void ncsc_replace_names(){

      // Replace all the names using ncsc_replace_name for inputs and output
      ncsc_replace_name(clk,"clk");             // Port 0
      ncsc_replace_name(reset,"reset");         // Port 1
      ncsc_replace_name(mem_addr,"mem_addr");           // Port 2
      ncsc_replace_name(mem_din,"mem_din");             // Port 3
      ncsc_replace_name(mem_we,"mem_we");               // Port 4
      ncsc_replace_name(addr,"addr");           // Port 2
      ncsc_replace_name(din,"din");             // Port 3
      ncsc_replace_name(we,"we");               // Port 4
      ncsc_replace_name(io_in,"io_in");         // Port 5

      ncsc_replace_name(dout,"dout");           // Port 6
      ncsc_replace_name(io_out,"io_out");       // Port 7

   }

#endif


};


// Step 1b - End the Header File
#endif 
