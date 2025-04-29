/*********************************************
 * File name: top.cpp
 * Author: Matthew Morrison
 * Email: matt.morrison@nd.edu
 * Date: 4/1/2025
 *
 * This file contains the top module definitions
 * testing the albacore 
 *********************************************/


// Step 5 - Import the top.h file
#include "top.h"

// Step 6-5 - Set up the process method
void top::process(){

  { 
    HLS_DEFINE_PROTOCOL("reset");
    datapath_inst.reset_datapath_clear_regs();
    controller_inst.reset_control_clear_regs();
    memory_map_inst.reset_memory_map();
  }
  
  {       
    HLS_DEFINE_PROTOCOL("wait");
    wait();
  }

  while(true){

      // Step 7-7 - Read from the input ports
      // Need to put before comparing to in_mem_we and in_start
      {
	HLS_DEFINE_PROTOCOL("input");
	
	// We will come back here
	// read_inputs();
	
      }    
      
      
      
      // Step 8-5 - Define the output protocol
      {
	// Step 8-5a - Write the protocol
	HLS_DEFINE_PROTOCOL("output");
	
	// write_outputs();
	mem_data_out.write(out_mem_data_out); // Port 6
	io_out.write(out_io_out);             // Port 7
	
      }
     
  
      // Define the final protocol block in the synthesizable blobk
      {       
	HLS_DEFINE_PROTOCOL("wait");
	wait();
      }
      
  }

}

void top::read_inputs(){

  // in_start = start.read();
  // in_mem_addr = mem_addr.read();        // Port 3
  // in_mem_data = mem_data.read();        // Port 4
  // in_mem_we = mem_we.read();            // Port 5
  // in_io_in = io_in.read();              // Port 6

  in_start.write(start.read());
  in_mem_addr.write(mem_addr.read());        // Port 3
  in_mem_data.write(mem_data.read());        // Port 4
  in_mem_we.write(mem_we.read());            // Port 5
  in_io_in.write(io_in.read());              // Port 6


}


void top::write_outputs(){

  mem_data_out.write(out_mem_data_out); // Port 6
  io_out.write(out_io_out);             // Port 7
  
}

// Change tb_datapath to tb_top, and then change all instances
// of datapath_dut to datapath_inst, and then delete 
// the previous out signals, since they are not the tb_top signals 
bool top::test_reset_datapath(){

  // Check the internal registers are set to 0

  if(datapath_inst.pc_reg != 0){
    cout << "datapath_inst.pc_reg - " << datapath_inst.pc_reg << endl;
    return false;
  }

  if(datapath_inst.instr_reg != 0){
    cout << "datapath_inst.instr_reg - " << datapath_inst.instr_reg << endl;
    return false;
  }

  if(datapath_inst.alu_a_reg != 0){
    cout << "datapath_inst.alu_a_reg - " << datapath_inst.alu_a_reg << endl;
    return false;
  }

  if(datapath_inst.alu_b_reg != 0){
    cout << "datapath_inst.alu_b_reg - " << datapath_inst.alu_b_reg << endl;
    return false;
  }
  if(datapath_inst.alu_result_reg != 0){
    cout << "datapath_inst.alu_result_reg - " << datapath_inst.alu_result_reg << endl;
    return false;
  }
  if(datapath_inst.mdr_reg != 0){
    cout << "datapath_inst.mdr_reg - " << datapath_inst.mdr_reg << endl;
    return false;
  }

  for( size_t iter = 0; iter < NUM_REGS; iter++){

    if(datapath_inst.register_file[iter] != 0){
      cout << "datapath_inst.register_file[" << iter << "] - " << datapath_inst.alu_a_reg << endl;
      return false;
    }

  }

  return true;
}


// Change tb_controller to tb_top, and then change all instances
// of controller_dut to top_dut.controller_inst, and then delete 
// the previous out signals, since they are not the tb_top signals 
bool top::test_reset_controller(){

  // Check the internal registers are set to 0
  if(controller_inst.state != 0){
    cout << "controller_inst.state - " << controller_inst.state << endl;
    return false;
  }

  if(controller_inst.next_state != 0){
    cout << "controller_inst.next_state - " << controller_inst.next_state << endl;
    return false;
  }


  return true;
}


// Change tb_memory_map to tb_top, and then change all instances
// of memory_map_dut to top_dut.memory_map_inst, and then delete 
// the previous out signals, since they are not the tb_top signals 
bool top::test_reset_memory_map(){

  for( size_t iter = 0; iter < RAM_SIZE; iter++ ){

    if(memory_map_inst.Memory[iter] != 0){
      cout << "Memory[" << iter << "] is not reset, equals " << memory_map_inst.Memory[iter] << endl;
      return false;
    }
  }

  return true;

}
