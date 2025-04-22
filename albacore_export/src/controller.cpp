#include "controller.h"
#include <iostream>

using namespace std;

controller::controller() {
  reset();
}

void controller::reset() {
  state = IFETCH;
  next_state = IFETCH;
  opcode_reg = 0;

  out_s_addr = false;
  out_enable_instr = false;
  out_enable_a = false;
  out_enable_b = false;
  out_alu_op = 0;
  out_enable_alu_result = false;
  out_s_regfile_din = false;
  out_reg_write = false;
  out_s_regfile_rw = false;
  out_enable_pc = false;
  out_we_mem = false;
  out_enable_mdr = false;
}

void controller::compute() {

  // Reset on start
  if (start || mem_we) {
    reset();
    return;
  }

  state = next_state;

  // Default outputs (cleared)
  out_s_addr = false;
  out_enable_instr = false;
  out_enable_a = false;
  out_enable_b = false;
  out_alu_op = 0;
  out_enable_alu_result = false;
  out_s_regfile_din = false;
  out_reg_write = false;
  out_s_regfile_rw = false;
  out_enable_pc = false;
  out_we_mem = false;
  out_enable_mdr = false;

  switch (state.to_uint()) {
    case IFETCH:
      reset();
      next_state = IFETCH2;
      break;
    case IFETCH2:
      out_enable_instr = true;
      next_state = DECODE;
      break;
    case DECODE:

      opcode_reg = opcode;

      out_enable_a = true;
      out_enable_b = true;
      switch (opcode_reg.to_uint()) {
        case 0: next_state = EX_ADD; break;
        case 1: next_state = EX_SUB; break;
        case 2: next_state = EX_AND; break;
        case 3: next_state = EX_OR; break;
        case 4: next_state = EX_NOT; break;
        case 5: next_state = EX_SHL; break;
        case 6: next_state = EX_SHR; break;
        case 7: next_state = EX_LDI; break;
        case 8: next_state = EX_LD; break;
        case 9: next_state = EX_ST; break;
        case 10: next_state = EX_BR; break;
        case 11: next_state = EX_BZ; break;
        case 12: next_state = EX_BN; break;
        case 13: next_state = EX_JAL; break;
        case 14: next_state = EX_JR; break;
        default: next_state = EX_QUIT; break;
      }
      break;

    case EX_ADD: case EX_SUB: case EX_AND: case EX_OR:
    case EX_NOT: case EX_SHL: case EX_SHR: case EX_LDI:
      out_alu_op = opcode_reg;
      out_enable_alu_result = true;
      next_state = WB_ALU;
      break;

    case EX_LD:
      out_alu_op = 8;
      out_enable_alu_result = true;
      next_state = MEM_LD;
      break;

    case EX_ST:
      out_alu_op = 9;
      out_enable_alu_result = true;
      next_state = MEM_ST;
      break;

    case EX_JAL:
      out_alu_op = 10;
      out_enable_alu_result = true;
      next_state = WB_JAL;
      break;

    case EX_BR:
      out_alu_op = 11;
      out_enable_pc = true;
      next_state = IFETCH;
      break;

    case EX_BZ:
      next_state = zero ? BR_TAKE : BR_NOT;
      break;

    case EX_BN:
      next_state = neg ? BR_TAKE : BR_NOT;
      break;

    case EX_JR:
      out_alu_op = 13;
      out_enable_pc = true;
      next_state = IFETCH;
      break;

    case MEM_LD:
      out_s_addr = true;
      next_state = MEM_LD2;
      break;

    case MEM_LD2:
      out_enable_mdr = true;
      next_state = WB_LD;
      break;

    case MEM_ST:
      out_we_mem = true;
      out_s_addr = true;
      out_alu_op = 10;
      out_enable_pc = true;
      next_state = MEM_ST2;
      break;

  case MEM_ST2:
    out_we_mem = true;
    next_state = IFETCH;
    break;

    case WB_ALU:

      out_s_regfile_rw = false;
      out_s_regfile_din = false;
      out_reg_write = true;
      out_alu_op = 10;
      out_enable_pc = true;
      next_state = IFETCH;

      break;

    case WB_LD:
      out_s_regfile_rw = false;
      out_s_regfile_din = true;
      out_reg_write = true;
      out_alu_op = 10;
      out_enable_pc = true;
      next_state = IFETCH;
      break;

    case WB_JAL:
      out_s_regfile_rw = true;
      out_s_regfile_din = false;
      out_reg_write = true;
      out_alu_op = 12;
      out_enable_pc = true;
      next_state = IFETCH;
      break;

    case BR_TAKE:
      out_alu_op = 11;
      out_enable_pc = true;
      next_state = IFETCH;
      break;

    case BR_NOT:
      out_alu_op = 10;
      out_enable_pc = true;
      next_state = IFETCH;
      break;

    case EX_QUIT:
      next_state = EX_QUIT;
      break;

    default:
      next_state = IFETCH;
      break;
  }
}

void controller::commit() {

}

bool controller::test_reset_controller(){

  // Check the internal registers are set to 0
  if(state != 0){
    cout << "controller_dut.state - " << state << endl;
    return false;
  }

  if(next_state != 0){
    cout << "controller_dut.next_state - " << next_state << endl;
    return false;
  }

  // Port 7
  if(out_s_addr != 0 ){ 
    cout << "out_s_addr error - " << out_s_addr << endl;
    return false;
  }

  // Port 8
  if(out_enable_instr != 0 ){
    cout << "out_enable_instr error - " << out_enable_instr << endl;
    return false;
  }

  // Port 9
  if(out_enable_a != 0 ){
    cout << "out_enable_a error - " << out_enable_a << endl;
    return false;
  }

  // Port 10
  if(out_enable_b != 0 ){
    cout << "out_enable_b error - " << out_enable_b << endl;
    return false;
  }

  // Port 11
  if(out_alu_op != 0 ){
    cout << "out_alu_op error - " << out_alu_op << endl;
    return false;
  }


  // Port 12
  if(out_enable_alu_result != 0 ){
    cout << "out_enable_alu_result error - " << out_enable_alu_result << endl;
    return false;
  }

  // Port 13
  if(out_s_regfile_din != 0 ){
    cout << "out_s_regfile_din error - " << out_s_regfile_din << endl;
    return false;
  }

  // Port 14
  if(out_reg_write != 0 ){
    cout << "out_reg_write error - " << out_reg_write << endl;
    return false;
  }

  // Port 15
  if(out_s_regfile_rw != 0 ){
    cout << "out_s_regfile_rw error - " << out_s_regfile_rw << endl;
    return false;
  }

  // Port 16
  if(out_enable_pc != 0 ){
    cout << "out_enable_pc error - " << out_enable_pc << endl;
    return false;
  }

  //Port 17
  if(out_we_mem != 0 ){
    cout << "out_we_mem error - " << out_we_mem << endl;
    return false;
  }

  // Port 18
  if(out_enable_mdr != 0 ){
    cout << "out_enable_mdr error - " << out_enable_mdr << endl;
    return false;
  }

  return true;
}
