#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "systemc.h"
#include "sizes.h"



class controller {
public:
  // Inputs
  bool start;
  bool mem_we;
  sc_bv<4> opcode;
  bool zero;
  bool neg;

  // FSM state
  sc_bv<5> state;
  sc_bv<5> next_state;
  sc_bv<4> opcode_reg;

  // Outputs (committed each cycle)
  bool out_s_addr;
  bool out_enable_instr;
  bool out_enable_a;
  bool out_enable_b;
  sc_bv<ALU_OP_W> out_alu_op;
  bool out_enable_alu_result;
  bool out_s_regfile_din;
  bool out_reg_write;
  bool out_s_regfile_rw;
  bool out_enable_pc;
  bool out_we_mem;
  bool out_enable_mdr;

  // Constructor
  controller();
  void reset();

  // Step methods
  void compute();
  void commit();

  // Reset test
  bool test_reset_controller();

private:


  // Latched outputs from compute()
  bool s_addr_next;
  bool enable_instr_next;
  bool enable_a_next;
  bool enable_b_next;
  sc_bv<ALU_OP_W> alu_op_next;
  bool enable_alu_result_next;
  bool s_regfile_din_next;
  bool reg_write_next;
  bool s_regfile_rw_next;
  bool enable_pc_next;
  bool we_mem_next;
  bool enable_mdr_next;
};

#endif
