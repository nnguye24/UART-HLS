/**************************************************************
 * File Name: top.h
 * Author: Matthew Morrison
 * Date: 4/1/2025
 *
 * Contains the SystemC Module Header for the albacore top-level module
 **************************************************************/

// Step 1a - Define the Header File
#ifndef __TOP_H__
#define __TOP_H__

// Step 2 - Include the libraries
#include "systemc.h" // SystemC Library
#include "stratus_hls.h" // Cadence Stratus

// Step 3 - Call sizes.h
#include "sizes.h"

// Step 3-1 - Call all the .h files
#include "datapath.h"
#include "controller.h"
#include "memory_map.h"


// Step 4 Defining the SC_MODULE
SC_MODULE(top){

  // Step 4-1 - Define the inputs and outputs to the datapath
  sc_in<bool> clk;                        // Port 0
  sc_in<bool> reset;                      // Port 1
  sc_in<bool> start;                      // Port 2
  sc_in< sc_bv<ADDR_W> > mem_addr;        // Port 3
  sc_in< sc_bv<DATA_W> > mem_data;        // Port 4
  sc_in<bool> mem_we;                     // Port 5
  sc_in< sc_bv<4> > io_in;                // Port 6
 
  sc_out< sc_bv<DATA_W> > mem_data_out;   // Port 7
  sc_out< sc_bv<4> > io_out;              // Port 8


  // Step 5 - Create all internal elements
  datapath datapath_inst;
  controller controller_inst;
  memory_map memory_map_inst;

  // Create Intermediate Signals

  // Datapath Input connections as sc_signals
  sc_signal< sc_bv<ALU_OP_W> > ctrl_to_dp_alu_op;      // Port 11 to Port 4
  sc_signal< sc_bv<DATA_W> > mem_to_dp_data_in;        // Port 8 to Port 5
  sc_signal<bool> ctrl_to_dp_enable_instruction;   // Port 8 to Port 6
  sc_signal<bool> ctrl_to_dp_enable_a;             // Port 9 to Port 7
  sc_signal<bool> ctrl_to_dp_enable_b;             // Port 10 to Port 8
  sc_signal<bool> ctrl_to_dp_enable_pc;            // Port 16 to Port 9  
  sc_signal<bool> ctrl_to_dp_enable_alu_result;    // Port 12 to Port 10 
  sc_signal<bool> ctrl_to_dp_enable_mdr;           // Port 18 to Port 11
  sc_signal<bool>  ctrl_to_dp_s_regfile_rw;         // Port 15 to Port 12 
  sc_signal<bool>  ctrl_to_dp_s_regfile_din;        // Port 13 to Port 13  
  sc_signal<bool>  ctrl_to_dp_reg_write;            // Port 14 to Port 14   
  sc_signal<bool>  ctrl_to_dp_s_addr;               // Port 7 to Port 15

  // Datapath outputs to MUX
  sc_signal< sc_bv<DATA_W> > dp_to_mem_data_out;       // Port 17
  sc_signal< sc_bv<ADDR_W> > dp_to_mem_addr_out;       // Port 18

  // Controller Inputs
  sc_signal< sc_bv<ALU_OP_W> > dp_to_ctrl_opcode;    // Port 16 to Port 4
  sc_signal<bool> dp_to_ctrl_zero;                   // Port 19 to Port 5
  sc_signal<bool> dp_to_ctrl_neg;                    // Port 20 to Port 6

  // Controller outputs to MUX
  sc_signal<bool>  ctrl_to_mem_we_mem;              // Port 17

  // Step 6-4 - Create a method called process to run the thread
  void process();

  void read_inputs();
  void write_outputs();

  // Copy the *test* method name from tb_datapath, tb_controller,and tb_memory_map
  bool test_reset_datapath();
  bool test_reset_controller();
  bool test_reset_memory_map();


  // Step 8-2 - Create the intermediate values from the inputs/outputs from the SC_MODULE
  //No clk or reset use sc_bit for bool
  sc_signal<bool> in_start;                       // Port 2
  sc_signal<sc_bv<ADDR_W>> in_mem_addr;         // Port 3
  sc_signal<sc_bv<DATA_W>> in_mem_data;         // Port 4
  sc_signal<bool> in_mem_we;                      // Port 5
  sc_signal<sc_bv<4>> in_io_in;                 // Port 6
  
  sc_signal< sc_bv<DATA_W> > out_mem_data_out;                 // Port 7
  sc_signal< sc_bv<4> > out_io_out;               // Port 8

  
  // Step 6-1 - Define the Constructor - Create all internal elements in the member initialization list
 SC_CTOR(top) : datapath_inst("datapath_inst"), controller_inst("controller_inst"), memory_map_inst("memory_map_inst") {
    
    // Step 6-2 - Define the Clocked thread
    SC_THREAD(process);
    
    // Step 6-3 - Define the Asynchronous reset
    sensitive << clk.pos();

    async_reset_signal_is(reset, false);

    // Binding input ports to internal signals using combinational logic
    SC_METHOD(read_inputs); 
    sensitive << start << mem_addr << mem_data << mem_we << io_in;
    
    // SC_METHOD(write_outputs); 
    // sensitive << out_mem_data_out << out_io_out;

   // Connect all the Datapath Signals
    datapath_inst.clk(clk);                                          // Port 0
    datapath_inst.reset(reset);                                      // Port 1
    datapath_inst.start(in_start);                                      // Port 2
    datapath_inst.mem_we(in_mem_we);                                    // Port 3
    datapath_inst.alu_op(ctrl_to_dp_alu_op);                         // Port 4
    datapath_inst.data_in(out_mem_data_out);                        // Port 5 ** Changed
    datapath_inst.enable_instruction(ctrl_to_dp_enable_instruction); // Port 6
    datapath_inst.enable_a(ctrl_to_dp_enable_a);                     // Port 7
    datapath_inst.enable_b(ctrl_to_dp_enable_b);                     // Port 8
    datapath_inst.enable_pc(ctrl_to_dp_enable_pc);                   // Port 9
    datapath_inst.enable_alu_result(ctrl_to_dp_enable_alu_result);   // Port 10
    datapath_inst.enable_mdr(ctrl_to_dp_enable_mdr);                 // Port 11
    datapath_inst.s_regfile_rw(ctrl_to_dp_s_regfile_rw);             // Port 12
    datapath_inst.s_regfile_din(ctrl_to_dp_s_regfile_din);           // Port 13
    datapath_inst.reg_write(ctrl_to_dp_reg_write);                   // Port 14
    datapath_inst.s_addr(ctrl_to_dp_s_addr);                         // Port 15
    datapath_inst.opcode(dp_to_ctrl_opcode);                         // Port 16
    datapath_inst.data_out(dp_to_mem_data_out);                      // Port 17
    datapath_inst.addr_out(dp_to_mem_addr_out);                      // Port 18
    datapath_inst.zero(dp_to_ctrl_zero);                             // Port 19
    datapath_inst.neg(dp_to_ctrl_neg);                               // Port 20
  
    // Connect all the Controller Signals
    controller_inst.clk(clk);                                        // Port 0
    controller_inst.reset(reset);                                    // Port 1
    controller_inst.start(in_start);                                    // Port 2
    controller_inst.mem_we(in_mem_we);                                  // Port 3
    controller_inst.opcode(dp_to_ctrl_opcode);                       // Port 4
    controller_inst.zero(dp_to_ctrl_zero);                           // Port 5
    controller_inst.neg(dp_to_ctrl_neg);                             // Port 6
    controller_inst.s_addr(ctrl_to_dp_s_addr);                       // Port 7
    controller_inst.enable_instr(ctrl_to_dp_enable_instruction);     // Port 8
    controller_inst.enable_a(ctrl_to_dp_enable_a);                   // Port 9
    controller_inst.enable_b(ctrl_to_dp_enable_b);                   // Port 10    
    controller_inst.alu_op(ctrl_to_dp_alu_op);                       // Port 11
    controller_inst.enable_alu_result(ctrl_to_dp_enable_alu_result); // Port 12
    controller_inst.s_regfile_rw(ctrl_to_dp_s_regfile_rw);           // Port 13
    controller_inst.reg_write(ctrl_to_dp_reg_write);                 // Port 14
    controller_inst.s_regfile_din(ctrl_to_dp_s_regfile_din);         // Port 15
    controller_inst.enable_pc(ctrl_to_dp_enable_pc);                 // Port 16
    controller_inst.we_mem(ctrl_to_mem_we_mem);                      // Port 17
    controller_inst.enable_mdr(ctrl_to_dp_enable_mdr);               // Port 18

    // Connect the inputs and outputs of the memory choice
  
    // Connect all the Memory Map signals
    memory_map_inst.clk(clk);                                        // Port 0
    memory_map_inst.reset(reset);                                    // Port 1
    memory_map_inst.mem_addr(in_mem_addr);                           // Port 2
    memory_map_inst.mem_din(in_mem_data);                            // Port 3
    memory_map_inst.mem_we(in_mem_we);                               // Port 4
    memory_map_inst.addr(dp_to_mem_addr_out);                           // Port 2
    memory_map_inst.din(dp_to_mem_data_out);                            // Port 3
    memory_map_inst.we(ctrl_to_mem_we_mem);                               // Port 4
    memory_map_inst.io_in(in_io_in);                                 // Port 5
    memory_map_inst.dout(out_mem_data_out);                         // Port 6
    memory_map_inst.io_out(out_io_out);                              // Port 7

  }

  // Step 4-2 - Implement NC_SYSTEMC
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
