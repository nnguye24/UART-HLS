/*********************************************
 * File name: datapath.cpp
 * Author: Matthew Morrison
 * Date: 4/19/2025
 *
 * This file contains the function definitions for
 * the datapath portion of the albacore processor
 *********************************************/

#include "datapath.h"
#include <iostream>

datapath::datapath() {
  reset();
}

void datapath::reset() {
  pc_reg = 0;
  instr_reg = 0;
  alu_a_reg = 0;
  alu_b_reg = 0;
  alu_result_reg = 0;
  mdr_reg = 0;
  for (int i = 0; i < NUM_REGS; ++i)
    register_file[i] = 0;

  decode_rb = 0;
  decode_ra = 0;
  decode_rw = 0;
  reg_data = 0;

  out_opcode = 0;
  out_data_out = 0;
  out_addr_out = 0;
  out_zero = true;
  out_neg = false;
}


/*******************************************************
 * Method name: compute
 * This method writes the previous cycle's
 * out_addr_out and out_data_out for the
 * memory_map to write the previou cycle's calculations
 * on store operation
 *******************************************************/
void datapath::compute() {
    out_addr_out = s_addr ? alu_result_reg : pc_reg;
    out_data_out = alu_a_reg;
}


/*******************************************************
 * Method name: commit
 * This methos calculates the next state logic and ALU 
 * operations based upon the control signals from the controller
 * and the data from the memory_map calculated in their compute methods
 ******************************************************/
void datapath::commit() {

  // === NEXT STATE LOGIC ===

  if (!start && !mem_we) {

    sc_bv<4> decode_rw = instr_reg.range(11, 8);
    sc_bv<4> decode_ra = instr_reg.range(7, 4);
    sc_bv<4> decode_rb = instr_reg.range(3, 0);
    sc_bv<8> immediate = instr_reg.range(7, 0);
    sc_bv<12> immed12 = instr_reg.range(11, 0);
    sc_bv<8> disp = instr_reg.range(11, 4);
    sc_bv<4> offset_load = instr_reg.range(7, 4);
    sc_bv<4> offset_store = instr_reg.range(11, 8);

    sc_bv<DATA_W> load_intermediate = (sc_bv<12>(0), offset_load);

    sc_bv<DATA_W> store_intermediate = (sc_bv<12>(0), offset_store);

    sc_bv<DATA_W> alu_a = alu_a_reg;
    sc_bv<DATA_W> alu_b = alu_b_reg;
    sc_bv<DATA_W> alu_pc = pc_reg;
    sc_bv<DATA_W> alu_result;

    if (reg_write) {
      decode_rw = s_regfile_rw ? sc_bv<4>(15) : decode_rw;
      register_file[decode_rw.to_uint()] = s_regfile_din ? mdr_reg : alu_result_reg;
    }

    switch (alu_op.to_uint()) {
    case 0: alu_result = alu_a.to_int() + alu_b.to_int(); break;
    case 1: alu_result = alu_a.to_int() - alu_b.to_int(); break;
    case 2: alu_result = alu_a & alu_b; break;
    case 3: alu_result = alu_a | alu_b; break;
    case 4: alu_result = ~alu_a; break;
    case 5: alu_result = alu_a << alu_b.range(3,0).to_uint(); break;
    case 6: alu_result = alu_a >> alu_b.range(3,0).to_uint(); break;
    case 7: alu_result = (sc_bv<8>(0), immediate); break;
    case 8: alu_result = alu_b.to_uint() + load_intermediate.to_uint(); break;
    case 9: alu_result = alu_b.to_uint() + store_intermediate.to_uint(); break;
    case 10: alu_result = alu_pc.to_uint() + 1; break;
    case 11: alu_result = alu_pc.to_uint() + ((disp[7].to_bool() ? sc_bv<8>(255) : sc_bv<8>(0)), disp).to_uint(); break;
    case 12: alu_result = (alu_pc.range(15, 12), immed12); break;
    case 13: alu_result = alu_a; break;
    case 14: alu_result = alu_pc.to_uint();
    default: alu_result = 0; break;
    }
    
    if (enable_a) alu_a_reg = register_file[decode_ra.to_uint()];
    if (enable_b) alu_b_reg = register_file[decode_rb.to_uint()];
    if (enable_alu_result) alu_result_reg = alu_result;
    if (enable_pc) pc_reg = alu_result;
    if (enable_mdr) mdr_reg = data_in;
 
    // === OUTPUTS ===
    out_addr_out = s_addr ? alu_result_reg : pc_reg;
    out_data_out = alu_a_reg;
    out_zero = (alu_b_reg == 0);
    out_neg = alu_b_reg[DATA_W - 1] == '1';   

  }
  /**************/
  if (enable_instruction) {
    instr_reg = data_in;
    alu_a_reg = 0;
    alu_b_reg = 0;
    alu_result_reg = 0;
    mdr_reg = 0;
    out_opcode = data_in.range(15, 12);
  }

}

bool datapath::test_reset_datapath(){

  // Check the internal registers are set to 0
  if(pc_reg != 0){
    std::cout << "pc_reg - " << pc_reg << std::endl;
    return false;
  }

  if(instr_reg != 0){
    std::cout << "instr_reg - " << instr_reg << std::endl;
    return false;
  }

  if(alu_a_reg != 0){
    std::cout << "alu_a_reg - " << alu_a_reg << std::endl;
    return false;
  }

  if(alu_b_reg != 0){
    std::cout << "alu_b_reg - " << alu_b_reg << std::endl;
    return false;
  }
  if(alu_result_reg != 0){
    std::cout << "alu_result_reg - " << alu_result_reg << std::endl;
    return false;
  }
  if(mdr_reg != 0){
    std::cout << "mdr_reg - " << mdr_reg << std::endl;
    return false;
  }

  for( size_t iter = 0; iter < NUM_REGS; iter++){

    if(register_file[iter] != 0){
      std::cout << "register_file[" << iter << "] - " << alu_a_reg << std::endl;
      return false;
    }

  }

  return true;
}
