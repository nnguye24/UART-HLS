/**************************************************************
 * File Name: datapath.h
 * Author: Matthew Morrison
 * Date: 4/1/2025
 *
 * Contains the SystemC Module Header for the albacore Datapath
 **************************************************************/

// Step 1a - Define the Header File
#ifndef __DATAPATH_H__
#define __DATAPATH_H__

// Step 2 - Include the libraries
#include "systemc.h" // SystemC Library
#include "stratus_hls.h" // Cadence Stratus

// Step 3 - Call sizes.h - Create a list of compiler directives
#include "sizes.h"

// Step 4 Defining the SC_MODULE
SC_MODULE(datapath){

  // Step 4-1 - Define the inputs and outputs to the datapath
  sc_in<bool> clk;                        // Port 0
  sc_in<bool> reset;                      // Port 1

  sc_in<bool> start;                      // Port 2
  sc_in<bool> mem_we;                     // Port 3 

  sc_in< sc_bv<ALU_OP_W> > alu_op;        // Port 4
  sc_in< sc_bv<DATA_W> > data_in;         // Port 5

  // Instruction Fetch Input
  sc_in<bool> enable_instruction;         // Port 6

  // Instruction Decode Inputs
  sc_in<bool> enable_a;                   // Port 7
  sc_in<bool> enable_b;                   // Port 8

  // Execute Inputs
  sc_in<bool> enable_pc;                  // Port 9
  sc_in<bool> enable_alu_result;          // Port 10

  // Instruction Load/Store Inputs
  sc_in<bool> enable_mdr;                 // Port 11
  sc_in<bool> s_regfile_rw;               // Port 12
  sc_in<bool> s_regfile_din;              // Port 13

  // Regitser Write Input
  sc_in<bool> reg_write;                  // Port 14

  sc_in<bool> s_addr;                     // Port 15

  sc_out< sc_bv<ALU_OP_W> > opcode;       // Port 16
  sc_out< sc_bv<DATA_W> > data_out;       // Port 17
  sc_out< sc_bv<ADDR_W> > addr_out;       // Port 18

  sc_out<bool> zero;                      // Port 19
  sc_out<bool> neg;                       // Port 20


  // Step 5 - Create all the internal registers
  sc_bv<ADDR_W> pc_reg;    // Program Counter
  sc_bv<DATA_W> instr_reg; // Instruction Regitser
  sc_bv<DATA_W> alu_a_reg; // ALU A Output Register
  sc_bv<DATA_W> alu_b_reg; // ALU B Output Register
  sc_bv<DATA_W> alu_result_reg;     // ALU Result Register
  sc_bv<DATA_W> mdr_reg;   // Memory-Data Register

  sc_bv<DATA_W> register_file[NUM_REGS]; // Register File

  // Step 7-1 - Create a method called process to run the thread
  void process();

  // Step X-X - The unique functions when you define the,
  void reset_datapath_clear_regs();  // Reset Cycle 1
  void read_inputs();
  void datapath_ops();
  void write_outputs();

  bool test_reset_datapath();

    // Step 8-2 - Create the intermediate values from the inputs/outputs from the SC_MODULE - No clk or reset use sc_bit for bool
  sc_bit in_start;               // Port 2
  sc_bit in_mem_we;              // Port 3
  sc_bv<ALU_OP_W> in_alu_op;     // Port 4
  sc_bv<DATA_W> in_data_in;      // Port 5 
  sc_bit in_enable_instruction;  // Port 6
  sc_bit in_enable_a;            // Port 7
  sc_bit in_enable_b;            // Port 8
  sc_bit in_enable_pc;           // Port 9
  sc_bit in_enable_alu_result;   // Port 10
  sc_bit in_enable_mdr;          // Port 11
  sc_bit in_s_regfile_rw;        // Port 12
  sc_bit in_s_regfile_din;       // Port 13
  sc_bit in_reg_write;           // Port 14
  sc_bit in_s_addr;              // Port 15

  sc_bv<ALU_OP_W> out_opcode;    // Port 16
  sc_bv<DATA_W> out_data_out;    // Port 17
  sc_bv<ADDR_W> out_addr_out;    // Port 18
  sc_bit out_neg;                // Port 19
  sc_bit out_zero;               // Port 20


  // Step 8 - Define the Constructor
  SC_CTOR(datapath){
    // Step 8-1 - Define the thread
    SC_THREAD(process);

    sensitive << clk.pos();

    async_reset_signal_is(reset, false);

  }

  // Step 4-2 - Implement NC_SYSTEMC
#ifdef NC_SYSTEMC
 public:
  void ncsc_replace_names(){

    // Replace all the names using ncsc_replace_name for inputs and output

    // Inputs
    ncsc_replace_name(clk, "clk");                               // Port 0
    ncsc_replace_name(reset, "reset");                           // Port 1
    ncsc_replace_name(start, "start");                           // Port 2
    ncsc_replace_name(mem_we, "mem_we");                         // Port 3
    ncsc_replace_name(alu_op, "alu_op");                         // Port 4
    ncsc_replace_name(data_in, "data_in");                       // Port 5
    ncsc_replace_name(enable_instruction, "enable_instruction"); // Port 6

    ncsc_replace_name(enable_a, "enable_a");                     // Port 7
    ncsc_replace_name(enable_b, "enable_b");                     // Port 8
    ncsc_replace_name(enable_pc, "enable_pc");                   // Port 9
    ncsc_replace_name(enable_alu_result, "enable_alu_result");   // Port 10
    ncsc_replace_name(enable_mdr, "enable_mdr");                 // Port 11

    ncsc_replace_name(s_regfile_rw, "s_regfile_rw");             // Port 12
    ncsc_replace_name(s_regfile_din, "s_regfile_din");           // Port 13
    ncsc_replace_name(reg_write, "reg_write");                   // Port 14
    ncsc_replace_name(s_addr, "s_addr");                         // Port 15

    // Outputs
    ncsc_replace_name(opcode, "opcode");                         // Port 16
    
    ncsc_replace_name(data_out, "data_out");                     // Port 17
    ncsc_replace_name(addr_out, "addr_out");                     // Port 18

    ncsc_replace_name(zero, "zero");                             // Port 19
    ncsc_replace_name(neg, "neg");                               // Port 20
    
  }

#endif

}; 


// Step 1b - End the Header File
#endif 
