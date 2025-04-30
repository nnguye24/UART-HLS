/**************************************************************
 * File Name: controller.h
 * Authoror: Matthew Morrison
 * Date: 4/1/2025
 *
 * Contains the SystemC Module Header for the albacore Controller
 **************************************************************/

// Step 1a - Define the Header File
#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

// Step 2 - Include the libraries
#include "systemc.h" // SystemC Library
#include "stratus_hls.h" // Cadence Stratus

// Step 3 - Call sizes.h
#include "sizes.h"

// Step 4 Defining the SC_MODULE
SC_MODULE(controller){

  // Step 4-1 - Define the inputs and outputs to the datapath
  sc_in<bool> clk;                        // Port 0
  sc_in<bool> reset;                      // Port 1

  sc_in<bool> start;                      // Port 2
  sc_in<bool> mem_we;                     // Port 3 

  sc_in< sc_bv<4> > opcode;               // Port 4
  sc_in<bool> zero;                       // Port 5
  sc_in<bool> neg;                        // Port 6

  sc_out<bool> s_addr;                    // Port 7
  sc_out<bool> enable_instr;              // Port 8
  sc_out<bool> enable_a;                  // Port 9
  sc_out<bool> enable_b;                  // Port 10
  sc_out< sc_bv<ALU_OP_W> > alu_op;       // Port 11
  sc_out<bool> enable_alu_result;         // Port 12
  sc_out<bool> s_regfile_din;             // Port 13
  sc_out<bool> reg_write;                 // Port 14
  sc_out<bool> s_regfile_rw;              // Port 15
  sc_out<bool> enable_pc;                 // Port 16
  sc_out<bool> we_mem;                    // Port 17
  sc_out<bool> enable_mdr;                // Port 18



  // Step 5 - Create all the internal registerss
  sc_bv<5> state;
  sc_bv<5> next_state;
  sc_bv<4> opcode_reg;

  // Step 6-4 - Create a function called process to run the thread
  void process();

  void reset_control_clear_regs();  // Reset Cycle 1

  void read_inputs();

  void clear_output_sc_bits();

  // The Unique Functions when you define them
  void controller_fsm();

  void write_outputs();


  bool test_reset_controller();

  // Step 8-2 - Create the intermediate values from the inputs/outputs from the SC_MODULE - No clk or reset 
  // Replace any bool with sc_bit
  sc_bit in_start;                      // Port 2
  sc_bit in_mem_we;                     // Port 3 

  sc_bv<ALU_OP_W> in_opcode;            // Port 4
  sc_bit in_zero;                       // Port 5
  sc_bit in_neg;                        // Port 6

  sc_bit out_s_addr;                    // Port 7
  sc_bit out_enable_instr;              // Port 8
  sc_bit out_enable_a;                  // Port 9
  sc_bit out_enable_b;                  // Port 10
  sc_bv<ALU_OP_W> out_alu_op;           // Port 11
  sc_bit out_enable_alu_result;         // Port 12
  sc_bit out_s_regfile_din;             // Port 13
  sc_bit out_reg_write;                 // Port 14
  sc_bit out_s_regfile_rw;              // Port 15
  sc_bit out_enable_pc;                 // Port 16
  sc_bit out_we_mem;                    // Port 17
  sc_bit out_enable_mdr;                // Port 18

  // Step 6-1 - Define the Constructor
  SC_CTOR(controller){

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

    // Inputs
    ncsc_replace_name(clk, "clk");                               // Port 0
    ncsc_replace_name(reset, "reset");                           // Port 1
    ncsc_replace_name(start, "start");                           // Port 2
    ncsc_replace_name(mem_we, "mem_we");                         // Port 3

    ncsc_replace_name(opcode, "opcode");                         // Port 4
    ncsc_replace_name(zero, "zero");                             // Port 5
    ncsc_replace_name(neg, "neg");                               // Port 6

    // Outputs
    ncsc_replace_name(s_addr, "s_addr");                         // Port 7
    ncsc_replace_name(enable_instr, "enable_instr");             // Port 8
    ncsc_replace_name(enable_a, "enable_a");                     // Port 9
    ncsc_replace_name(enable_b, "enable_b");                     // Port 10
    ncsc_replace_name(alu_op, "alu_op");                         // Port 11
    ncsc_replace_name(enable_alu_result, "enable_alu_result");   // Port 12
    ncsc_replace_name(s_regfile_din, "s_regfile_din");           // Port 13
    ncsc_replace_name(reg_write, "reg_write");                   // Port 14
    ncsc_replace_name(s_regfile_rw, "s_regfile_rw");             // Port 15
    ncsc_replace_name(enable_pc, "enable_pc");                   // Port 16
    ncsc_replace_name(we_mem, "we_mem");                         // Port 17
    ncsc_replace_name(enable_mdr, "enable_mdr");                 // Port 18

  }
#endif

};

// Step 1b - End the Header File
#endif 
