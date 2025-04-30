/**************************************************************
 * File Name: datapath.cpp
 * Author: Matthew Morrison
 * Date: 4/1/2025
 *
 * Contains the SystemC Methods for the albacore Datapath class
 **************************************************************/

// Step 6 - Import the datapath.h file
#include "datapath.h"

// Step 7-2 - Set up the process method
void datapath::process(){

  {
    HLS_DEFINE_PROTOCOL("reset");
    reset_datapath_clear_regs();
  }

  {       
    HLS_DEFINE_PROTOCOL("wait");
    wait();
  }
  
  // Step 7-3 - Create the While Loop
  while(true){

    // Step 7-7 - Read from the input ports
    // Need to put before comparing to in_mem_we and in_start
      
    
    // Define the final protocol block in the synthesizable blobk
    {       
      HLS_DEFINE_PROTOCOL("wait");

      in_start = start.read();
      wait();
    }

    {
      HLS_DEFINE_PROTOCOL("read_mem_we");

      // Read in the 
      in_mem_we = mem_we.read();  // Port 3
    }


    if( !in_mem_we ){
      
      if( in_start ){
	
	{
	  // Step 7-5
	  HLS_DEFINE_PROTOCOL("start");
	  
	  // We will come back here
	  reset_datapath_clear_regs();
	  
	}
      }
      
      else{   
	
	{
	  HLS_DEFINE_PROTOCOL("input");
	  
	  // Read in the remaining inputs
	  read_inputs();
	  
	}
	
	{
	  
	  HLS_DEFINE_PROTOCOL("datapath_ops");
	  
	  // Step 7-8 - Perform the datapath operations
	  // We will come back here
	  datapath_ops();
	  
	}
	
	
	
      }
     
      
    } // End the while loop

    {
      // 7-9 - Write to the output ports
      
      HLS_DEFINE_PROTOCOL("output");
      
      write_outputs();
    }
    
    
      // Define the final protocol block in the synthesizable blobk
    {       
      HLS_DEFINE_PROTOCOL("wait");
      wait();
    }
    
  }
  
}


void datapath::reset_datapath_clear_regs(){

    // Step 7-3 Reset any memory elements

    // We will set all the datapath registers to 0
    pc_reg = 0;
    instr_reg = 0;
    alu_a_reg = 0;
    alu_b_reg = 0;
    alu_result_reg = 0;
    mdr_reg = 0;

    // Set all 16 albacore registers in the register file to 0
    for(size_t i = 0; i < NUM_REGS; i++){
      register_file[i] = 0;
    }

    // Clear the outputs
    out_opcode = 0; // Port 16
    out_data_out = 0; // Port 17
    out_addr_out = 0; // Port 18
    out_zero = 1; // Port 19
    out_neg = 0;   // Port 20
  

}


void datapath::read_inputs(){

  // Step 8-3b Read the inputs
  // in_start = start.read();    // Port 2
  in_alu_op = alu_op.read();    // Port 4
  in_data_in = data_in.read();  // Port 5

  // Instruction Fetch Input
  in_enable_instruction = enable_instruction.read(); // Port 6
  
  // Instruction Decode Inputs
  in_enable_a = enable_a.read(); // Port 7
  in_enable_b = enable_b.read(); // Port 8
  
  // Execution Stage
  in_enable_pc = enable_pc.read(); // Port 9
  in_enable_alu_result = enable_alu_result.read(); // Port 10
  in_enable_mdr = enable_mdr.read(); // Port 11
  
  in_s_regfile_rw = s_regfile_rw.read();  // Port 12
  in_s_regfile_din = s_regfile_din.read(); // Port 13
  
  // Register Write Input
  in_reg_write = reg_write.read();      // Port 14
  in_s_addr = s_addr.read();            // Port 15
  
}

void datapath::write_outputs(){
  
  // Step 8.5b - Write to the outputs
  opcode.write(out_opcode); // Port 16
  data_out.write(out_data_out); // Port 17
  addr_out.write(out_addr_out); // Port 18
  zero.write(out_zero); // Port 19
  neg.write(out_neg);   // Port 20
  
}


void datapath::datapath_ops(){

  cout << "in_data_in " << in_data_in << " " << sc_time_stamp() << endl;

  ///////////////////////////////////////////////////////////////
  // FIRST HALF
  // Read in inputs 
  // Write to the register file to enable write, then read capability
  // Read from registers when then previous cycle's information is needed
  // in order to prevent data hazards
  //////////////////////////////////////////////////////////////
  
  ////////////////////////
  // Instruction FETCH - First Half
  ////////////////////////
  
  // Write the PC to the output to obtain the instruction itself

  if(in_s_addr){
    out_addr_out = alu_result_reg;
  }  

  
  ////////////////////////
  // Instruction FETCH 2
  ////////////////////////
  
  // If the enable is true, write data to the instruction register
  // This is read from the RAM, and will be used in the next cycle

  if( in_enable_instruction ){

    // We will also need to clear all the other intermediate registers
    // So that the previous instruction's results will not corrupt
    // The new instruction   
    alu_a_reg = 0; // ALU A Output Register
    alu_b_reg = 0; // ALU B Output Register
    alu_result_reg = 0;     // ALU Result Register
    mdr_reg = 0;   // Memory-Data Register

    instr_reg = in_data_in;

    // Update the opcode register

    out_opcode = instr_reg.range(15,12);

  }

  
  
  ////////////////////////
  // Instruction DECODE - Write in the First Half
  ////////////////////////
  
  // Get the instruction from the previous cycle 
  sc_bv<4> decode_rb = instr_reg.range(3,0);
  sc_bv<4> decode_ra = instr_reg.range(7,4);
  sc_bv<4> decode_rw;
  sc_bv<DATA_W> reg_data;
  
  // Write, then Read Capability - Pg. 311 of PLPD

  if(in_reg_write){
    
    // Set the decode rw address
    if(!in_s_regfile_rw){
      decode_rw = instr_reg.range(11,8);
    }
    else{
      decode_rw = sc_bv<4>(15);
    }
    
    // Set the value based on ALU or MDR from the previous cycle 
    if(in_s_regfile_din){
      reg_data = mdr_reg;
    }
    else{
      reg_data = alu_result_reg;
    }

    // Must set decode_rw to unsigned integer to access the array location
    register_file[decode_rw.to_uint()] = reg_data;
    
  }
  
  
  ////////////////////////
  // Execute - Write in the First Half
  ////////////////////////
  
  // Take inputs from the alu_a_reg, alu_b_reg, and pc_reg registers from the previous cycle
  sc_bv<DATA_W> alu_a = alu_a_reg;
  sc_bv<DATA_W> alu_b = alu_b_reg; 
  sc_bv<DATA_W> alu_pc = pc_reg;
  
  // ALU Output Wire
  sc_bv<DATA_W> alu_result;
  
  // Split up the Instruction Register that was just read
  sc_bv<8> immediate = instr_reg.range(7,0);
  sc_bv<12> immed12 = instr_reg.range(11,0);
  sc_bv<8> disp = instr_reg.range(11,4);
  
  sc_bv<4> offset_load = instr_reg.range(7,4);
  sc_bv<DATA_W> load_intermediate = ( (sc_bv<12>(0), offset_load) );
  
  sc_bv<4> offset_store = instr_reg.range(11,8); 
  sc_bv<DATA_W> store_intermediate = ( (sc_bv<12>(0), offset_store) );
  
  
  ////////////////////////////////////
  // LOAD / STORE - First Half 
  ///////////////////////////////////
  // The output of the ALU from the previous cycle is written to the address output for load and store
  // We are sending the address calculated in the previous cycle to the memory
  // out_addr_out = alu_result_reg;
  
  // The output of the A register from the previous cycle is written to the data for store
  // We are sending the data calculated in the previous cycle to the memory for a potential store
  out_data_out = alu_a_reg;
  
  
  ////////////////////////////////////
  ////////////////////////////////////
  // SECOND HALF
  // Calculate the results 
  // Update the intermediate registers for use in the next cycle
  ///////////////////////////////////
  ////////////////////////////////////
  
  
  
  /////////////////////////////////////
  // Instruction DECODE - Second Half
  /////////////////////////////////////
  
  // Write to the alu_a_reg and alu_b_reg for use in the next cycle 
  if(in_enable_a){
    alu_a_reg = register_file[decode_ra.to_uint()];
  }
  
  if(in_enable_b){
    alu_b_reg = register_file[decode_rb.to_uint()];
  }
  
	  
  ///////////////////////
  // Execute - Second Half
  ///////////////////////
  
  cout << in_alu_op << " " << sc_time_stamp() << endl;

  // ALU Operations
  switch( in_alu_op.to_uint() ){
    
    // Signed Addition
  case 0:
    alu_result = alu_a.to_int() + alu_b.to_int();
    break;
    
	      // Signed Subtraction
  case 1:
    alu_result = alu_a.to_int() - alu_b.to_int();
    break;
    
    // AND
  case 2:
    alu_result = alu_a & alu_b;
    break;
    
    // OR
  case 3:
    alu_result = alu_a | alu_b;
    break;
    
    // NOT
  case 4:
    alu_result = ~alu_a;
    break;
    
    // Shift Left Logical
  case 5:
    // Using range 3,0 for SRL prevents a warning in Synthesis 
    alu_result = alu_a << alu_b.range(3,0).to_uint();
    break;
    
    // Shift Right Logical
  case 6:
    // Using range 3,0 for SRL prevents a warning in Synthesis 
    alu_result = alu_a >> alu_b.range(3,0).to_uint();
    break;
    
    //{8'h00, imm};
  case 7:
    alu_result = ( (sc_bv<8>(0), immediate) );
    break;
    
    //  b + {8'd0, offset_ld}
  case 8:
    alu_result = alu_b.to_uint() + load_intermediate.to_uint();
    break;
    
  case 9:
    alu_result = alu_b.to_uint() + store_intermediate.to_uint();
    break;
    
    // Result = PC + 1
  case 10:
    alu_result = alu_pc.to_uint() + 1;
    break;
    
    // Sign extent disp and add to PC
  case 11: 
    alu_result = alu_pc.to_uint() + (disp[7].to_bool() ? (sc_bv<8>(255), disp) : (sc_bv<8>(0), disp)).to_uint();
    break;
    
    // PC[15:12] and immediate[11:0]
  case 12:
    alu_result = (alu_pc.range(15,12), immed12);
    break;
    
    // out = a
  case 13:
    alu_result = alu_a;
    break;
    
    // Default Case
  default:
    alu_result = 0;
    break;
    
  }
  
  // Outputs 
  // Put the output on ALU result Register if the ALU Enable is true
  if(in_enable_alu_result){
    alu_result_reg = alu_result;
  }
  else{
    alu_result_reg = sc_lv<DATA_W>(0);
  }
  
  out_neg = alu_b[DATA_W-1];
  out_zero = alu_b == 0;
  
  // If the PC is enabled, write the alu result to the PC Register
  if(in_enable_pc){
    pc_reg = alu_result;
  }

  if(!in_s_addr){
    out_addr_out = pc_reg;
  }
  
  
  /////////////////////////////////
  // LOAD STORE - Second Half
  /////////////////////////////////
  
  // If the enable is true, write data to the memory data register
  // Put in the second half so this is used in the next cycle 
  if( in_enable_mdr ){
    mdr_reg = in_data_in;
    cout << "in_enable_mdr " << mdr_reg << " " << in_data_in << endl;
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
