#ifndef __DATAPATH_H__
#define __DATAPATH_H__

#include "systemc.h"
#include "sizes.h"
#include "stratus_hls.h" // Cadence Stratus

class datapath {
 public:

  // Datapath inputs
  bool start;
  bool mem_we;
  sc_bv<ALU_OP_W> alu_op;
  sc_bv<DATA_W> data_in;
  bool enable_instruction;
  bool enable_a;
  bool enable_b;
  bool enable_pc;
  bool enable_alu_result;
  bool enable_mdr;
  bool s_regfile_rw;
  bool s_regfile_din;
  bool reg_write;
  bool s_addr;
  
  // Datapath Outputs
  sc_bv<ALU_OP_W> out_opcode;
  sc_bv<DATA_W> out_data_out;
  sc_bv<ADDR_W> out_addr_out;
  bool out_zero;
  bool out_neg;

  // Intermediate Datapath Registers
  sc_bv<ADDR_W> pc_reg;
  sc_bv<DATA_W> instr_reg;
  sc_bv<DATA_W> alu_a_reg;
  sc_bv<DATA_W> alu_b_reg;
  sc_bv<DATA_W> alu_result_reg;
  sc_bv<DATA_W> mdr_reg;
  sc_bv<DATA_W> register_file[NUM_REGS];

  sc_bv<4> decode_rb;
  sc_bv<4> decode_ra;
  sc_bv<4> decode_rw;
  sc_bv<DATA_W> reg_data;
  
  // Constructor
  datapath();
  
  // Reset method
  void reset();
  
  // Two-phase interface
  void compute();
  void commit();

  // Perform reset check
  bool test_reset_datapath();

};

#endif
