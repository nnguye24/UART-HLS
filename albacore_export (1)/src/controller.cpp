 /**************************************************************
 * File Name: controller.cpp
 * Authoror: Matthew Morrison
 * Date: 4/1/2025
 *
 * Contains the SystemC Module Definitions for the albacore Controller
 **************************************************************/

// Step 5 - Inport the controller.h file
#include "controller.h" 

// Step 6-5 - Set up the process method
void controller::process(){

  {
    HLS_DEFINE_PROTOCOL("reset");
    reset_control_clear_regs();
  }
  
  {       
    HLS_DEFINE_PROTOCOL("wait");
    wait();
  }

  while(true){

    // Step 8-3 - Define the input protocol
    {
      // Step 8-3a - Define the input protocol
      HLS_DEFINE_PROTOCOL("input");
      in_mem_we = mem_we.read();  // Port 3

      // Step 8-3b Read the inputs
      read_inputs();	  
      
    } // Step 8-3c - Don't forget the closing }
    
    
    
    if(!in_mem_we){
      
      if( in_start ){
	
	{
	  // Step 7-5
	  HLS_DEFINE_PROTOCOL("start");
	  
	  // We will come back here
	  reset_control_clear_regs();
	  
	  // Do not wait
	}
      }	
      
      else{
	
	// Step 7-8 - Perform the datapath operations
	// We will come back here

	{
	  HLS_DEFINE_PROTOCOL("controller_fsm");
	  controller_fsm();
	}

	{
	  // 7-9 - Write to the output ports
	  
	  HLS_DEFINE_PROTOCOL("output");
	  
	  write_outputs();
	}
	
      }
    }
  

    // Define the final protocol block in the synthesizable blobk
    {       
      HLS_DEFINE_PROTOCOL("wait");
      wait();
    }

    /// Do nothing, as the datapath will use this for processing time

    // Define the final protocol block in the synthesizable blobk
    {       
      HLS_DEFINE_PROTOCOL("wait");
      wait();
    }
   
  }
}

void controller::reset_control_clear_regs(){

  // Set all controller register to 0
  state = IFETCH;
  next_state = IFETCH;
  opcode_reg = 0;
  clear_output_sc_bits();

}

void controller::read_inputs(){

  in_start = start.read();    // Port 2
  in_opcode = opcode.read();  // Port 4
  in_zero = zero.read();      // Port 5
  in_neg = neg.read();        // Port 6
}


void controller::clear_output_sc_bits(){

  // Clear the outputs
  out_s_addr = 0;                    // Port 7
  out_enable_instr = 0;              // Port 8
  out_enable_a = 0;                  // Port 9
  out_enable_b = 0;                  // Port 10
  out_alu_op = 0;           // Port 11
  out_enable_alu_result = 0;         // Port 12
  out_s_regfile_din = 0;             // Port 13
  out_reg_write = 0;                 // Port 14
  out_s_regfile_rw = 0;              // Port 15
  out_enable_pc = 0;                 // Port 16
  out_we_mem = 0;                    // Port 17
  out_enable_mdr = 0;                // Port 18

}

void controller::controller_fsm(){

  if( in_mem_we ){

    // Do nothing
    return;
  }
  else if ( in_start ){

    state = IFETCH;
    next_state = IFETCH2;
    return;
  }

  else{

    state = next_state;

    // First, clear all the outputs from previous cycles
    clear_output_sc_bits();

    switch( state.to_uint() ){
      
    case IFETCH:
      out_s_addr = 0;
      opcode_reg = 0;
      next_state = IFETCH2;
      break;
      
    case IFETCH2:
      out_enable_instr = true;
      next_state = DECODE;
      break;

    case DECODE:
      opcode_reg = in_opcode;
      out_enable_instr = false;
      out_enable_a = true;
      out_enable_b = true;

      switch( opcode_reg.to_uint() ){

      case 0:
	next_state = EX_ADD;
	break;
      case 1:
	next_state = EX_SUB;
	break;
      case 2:
	next_state = EX_AND;
	break;
      case 3:
	next_state = EX_OR;
	break;
      case 4:
	next_state = EX_NOT;
	break;
      case 5:
	next_state = EX_SHL;
	break;
      case 6:
	next_state = EX_SHR;
	break;
      case 7:
	next_state = EX_LDI;
	break;
      case 8:
	next_state = EX_LD;
	break;
      case 9:
	next_state = EX_ST;
	break;
      case 10:
	next_state = EX_BR;
	break;
      case 11:
	next_state = EX_BZ;
	break;
      case 12:
	next_state = EX_BN;
	break;
      case 13:
	next_state = EX_JAL;
	break;
      case 14:
	next_state = EX_JR;
	break;
      default:
	next_state = EX_QUIT;
	break;
      }

      break;

    case EX_ADD:
      out_alu_op = 0;
      out_enable_alu_result = true;
      next_state = WB_ALU;
      break;

    case EX_SUB:
      out_alu_op = 1;
      out_enable_alu_result = true;
      next_state = WB_ALU;
      break;

    case EX_AND:
      out_alu_op = 2;
      out_enable_alu_result = true;
      next_state = WB_ALU;
      break;

    case EX_OR:
      out_alu_op = 3;
      out_enable_alu_result = true;
      next_state = WB_ALU;
      break;

    case EX_NOT:
      out_alu_op = 4;
      out_enable_alu_result = true;
      next_state = WB_ALU;
      break;

    case EX_SHL:
      out_alu_op = 5;
      out_enable_alu_result = true;
      next_state = WB_ALU;
      break;

    case EX_SHR:
      out_alu_op = 6;
      out_enable_alu_result = true;
      next_state = WB_ALU;
      break;

    case EX_LDI:
      out_alu_op = 7;
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
      if(in_zero)
	next_state = BR_TAKE;
      else
	next_state = BR_NOT;
      break;

    case EX_BN:
      if(in_neg)
	next_state = BR_TAKE;
      else
	next_state = BR_NOT;
      break;

    case EX_JR:
      out_alu_op = 13;
      out_enable_pc = true;
      next_state = IFETCH;
      break;

    case EX_QUIT:
      next_state = EX_QUIT;
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

    default:
      state = IFETCH;
      next_state = IFETCH;
      break;

    }
  }

}

void controller::write_outputs(){

  // Write to all the outputs
  s_addr.write(out_s_addr);                       // Port 7
  enable_instr.write(out_enable_instr);           // Port 8
  enable_a.write(out_enable_a);                   // Port 9
  enable_b.write(out_enable_b);                   // Port 10
  alu_op.write(out_alu_op);                       // Port 11
  enable_alu_result.write(out_enable_alu_result); // Port 12
  s_regfile_din.write(out_s_regfile_din);         // Port 13
  reg_write.write(out_reg_write);                 // Port 14
  s_regfile_rw.write(out_s_regfile_rw);           // Port 15
  enable_pc.write(out_enable_pc);                 // Port 16
  we_mem.write(out_we_mem);                       // Port 17
  enable_mdr.write(out_enable_mdr);               // Port 18

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
