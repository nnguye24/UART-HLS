/*********************************************
 * File name: sc_controller.cpp
 * Author: Matthew Morrison
 * Email: matt.morrison@nd.edu
 * Date: 4/3/2025
 *
 * This file contains the sc_main function for
 * testing the albacore controller
 *********************************************/

// Include the Unit under test and the test bench
#include "../src/controller.h"
#include "../tb/tb_controller.h"

void state_test( const tb_controller& tb_controller_inst, const sc_bv<5>& state, const sc_bv<5> next_state ){

  if( tb_controller_inst.controller_dut.state != state ){
    cout << "Unexpected state Register value of " << tb_controller_inst.controller_dut.state << endl;
    cout << "Start Test Failed" << endl;
    sc_stop();
    exit(-1);
  }
  else{
    cout << "Expected state Register value of " << tb_controller_inst.controller_dut.state << endl;
  }

  if( tb_controller_inst.controller_dut.next_state != next_state ){
    cout << "Unexpected next_state Register value of " << tb_controller_inst.controller_dut.next_state << endl;
    cout << "Start Test Failed" << endl;
    sc_stop();
    exit(-1);
  }
  else{
    cout << "Expected next_state Register value of " << tb_controller_inst.controller_dut.next_state << endl;
  }

}

int sc_main( int argc, char *argv[] ) {

  // Set the Clock and the Reet
  sc_clock   clk("clk", CYCLE_LENGTH, SC_NS);
  sc_signal<bool> reset;

  // Create the Controller Instance
  tb_controller tb_controller_inst("tb_controller_inst");

  // Connect the clock and the reet
  tb_controller_inst.clk(clk);
  tb_controller_inst.reset(reset);

  // Initializing the test
  cout << "Setting start to 0 and mem_we to 0 at " << sc_time_stamp() << endl;

  // Run two cycles with reset set to true so we properly test the reset
  cout << "-------------------------" << endl;
  cout << "Start up with 2 cycles" << endl; 
  cout << "-------------------------" << endl;
  reset.write(true);

  // Set start and mem_we to 0, since the program has not started and we are not writing to memory
  tb_controller_inst.in_start.write(0); 
  tb_controller_inst.in_mem_we.write(0); 

  sc_start(2*CYCLE_LENGTH,SC_NS);

  // Do a reset by setting to false and running 1 cycle
  cout << "-------------------------" << endl;
  cout << "Running a reset test" << endl;
  cout << "The reset block will take 1 cycle" << endl;
  cout << "-------------------------" << endl;
  reset.write(false);
  tb_controller_inst.in_mem_we.write(0); 
  sc_start(CYCLE_LENGTH,SC_NS);

  // Test the Controller Reset
  if(tb_controller_inst.test_reset_controller()){

    // If we got here, all the reset tests in tb_datapath passed
    cout << "Reset test PASSED" << endl;
  }
  else{
    cout << "Reset test FAILED" << endl;
    sc_stop();
    exit(EXIT_FAILURE);
  }

  cout << "-------------------------" << endl;

  // We will represent 8 cycles where the six instructions and
  // two memory address locations are being written.
  // No datapath operations should take place

  // Clear the reset
  reset.write(true);

  // Set mem_we to true. Start is still 0
  tb_controller_inst.in_mem_we.write(1); 

  cout << "Setting mem_we to 1 at " << sc_time_stamp() << endl;

  cout << "-------------------------" << endl;
  cout << "Writing to the Memory RAM for 8 cycles" << endl;
  cout << "-------------------------" << endl;

  // 0x7220 is being written to the RAM Memory[0]
  sc_start(CYCLE_LENGTH,SC_NS);

  // 0x8002 is being written to the RAM Memory[1]
  sc_start(CYCLE_LENGTH,SC_NS);

  // 0x8112 is being written to the RAM Memory[2]
  sc_start(CYCLE_LENGTH,SC_NS);

  // 0x0001 is being written to the RAM Memory[3]
  sc_start(CYCLE_LENGTH,SC_NS);

  // 0x9202 is being written to the RAM Memory[4]
  sc_start(CYCLE_LENGTH,SC_NS);

  // 0xF000 is being written to the RAM Memory[5]
  sc_start(CYCLE_LENGTH,SC_NS);

  // 0x0003 is being written to the RAM Memory[32]
  sc_start(CYCLE_LENGTH,SC_NS);

  // 0x0004 is being written to the RAM Memory[33]
  sc_start(CYCLE_LENGTH,SC_NS);


  cout << "-------------------------" << endl;
  cout << "Clearing reset and starting datapath tests" << endl;
  cout << "This test includes the six instructions from PLPD pg. 296" << endl;
  cout << "A compiling for the following C code:" << endl;
  cout << "int main(){" << endl;
  cout << "    A[0] = 2;" << endl;
  cout << "    A[1] = 3;" << endl;
  cout << "    A[2] = A[1]+A[0];" << endl;
  cout << "    return 0;" << endl;
  cout << "}" << endl;
  cout << "-------------------------" << endl;

  cout << "-------------------------" << endl;
  cout << "Testing Start Signal" << endl;

  // Starting the program
  // Set start to 1 and mem_we to 0
  tb_controller_inst.in_start.write(1); 
  tb_controller_inst.in_mem_we.write(0); 

  cout << "Setting start to 1 and mem_we to 0 at " << sc_time_stamp() << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  state_test( tb_controller_inst, IFETCH, IFETCH );
  
  
  cout << "Start Test Passed" << endl;


  // Instruction Fetch - Getting PC Address 0

  cout << "-------------------------" << endl;
  cout << "Testing 0x7220 - ldi $2 32" << endl;
  
  cout << "-------------------------" << endl;
  cout << "Instruction Fetch Cycle IFETCH" << endl;

  // Set start to 0 since the program has started
  tb_controller_inst.in_start.write(0);

  sc_start(CYCLE_LENGTH,SC_NS);

  if(tb_controller_inst.out_s_addr.read() != 0){

    cout << "Unexpected s_addr value of " << tb_controller_inst.out_s_addr.read() << endl;
    cout << "Start Test Failed" << endl;
    sc_stop();
    exit(-1);
  }
  else{
    cout << "Expected s_addr value of " << tb_controller_inst.out_s_addr.read() << endl;
  }


  cout << "-------------------------" << endl;
  cout << "Instruction Fetch Cycle IFETCH2" << endl;

  // Datapath is sending an opcode of 7 for LDI
  tb_controller_inst.in_opcode.write(7);

  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.controller_dut.opcode_reg != 7 ){

    cout << "Unexpected result of opcode - " << tb_controller_inst.controller_dut.opcode_reg << endl;
    sc_stop();
    exit(-1);
  }
  else{
    cout << "Expected result of opcode - " << tb_controller_inst.controller_dut.opcode_reg << endl;
  }

  state_test( tb_controller_inst, IFETCH2, DECODE );

  cout << "IFETCH2 test passed" << endl; 

  cout << "-------------------------" << endl;
  cout << "Instruction Decode Cycle" << endl;


  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.out_enable_instr.read() != 0 ||
      tb_controller_inst.out_enable_a.read() != 1 ||
      tb_controller_inst.out_enable_a.read() != 1 ){

    cout << "Controller DECODE outputs failed" << endl;
  }
  else{
    cout << "Controller DECODE outputs passed" << endl;
  }

  state_test( tb_controller_inst, DECODE, EX_LDI );

  cout << "Instruction DECODE cycle passed." << endl;

  cout << "-------------------------" << endl;
  cout << "Execution Cycle" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.out_alu_op.read() != 7 ||
      tb_controller_inst.out_enable_alu_result.read() != 1 ){

    cout << "Controller EX_LDI outputs failed" << endl;
  }
  else{
    cout << "Controller EX_LDI outputs passed" << endl;
  }


  state_test( tb_controller_inst, EX_LDI, WB_ALU );

  cout << "-------------------------" << endl;
  cout << "Load/Store Cycle - WB_ALU" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  // Test the controller output signals
  if(tb_controller_inst.out_s_regfile_rw.read() != 0 ||
     tb_controller_inst.out_s_regfile_din.read() != false ||
     tb_controller_inst.out_reg_write.read() != true ||
     tb_controller_inst.out_alu_op.read() != 10 ||
     tb_controller_inst.out_enable_pc.read() != true ){

    cout << "WB_ALU Failed" << endl;

  }
  else{
    cout << "WB_ALU Passed" << endl;
  }

  state_test( tb_controller_inst, WB_ALU, IFETCH );

  cout << "... Load/Store Test Passed!" << endl;

  cout << "-------------------------" << endl;
  cout << "-------------------------" << endl;
  cout << "Testing 0x8002 - ld $r0 $r2 0" << endl;
  
  cout << "-------------------------" << endl;
  cout << "Instruction Fetch Cycle IFETCH" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  if(tb_controller_inst.out_s_addr.read() != 0){

    cout << "Unexpected s_addr value of " << tb_controller_inst.out_s_addr.read() << endl;
    cout << "Start Test Failed" << endl;
    sc_stop();
    exit(-1);
  }
  else{
    cout << "Expected s_addr value of " << tb_controller_inst.out_s_addr.read() << endl;
  }

  cout << "-------------------------" << endl;
  cout << "Instruction Fetch Cycle IFETCH2" << endl;

  // Datapath is sending an opcode of 8 for LD
  tb_controller_inst.in_opcode.write(8);

  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.controller_dut.opcode_reg != 8 ){

    cout << "Unexpected result of opcode - " << tb_controller_inst.controller_dut.opcode_reg << endl;
    sc_stop();
    exit(-1);
  }
  else{
    cout << "Expected result of opcode - " << tb_controller_inst.controller_dut.opcode_reg << endl;
  }

  state_test( tb_controller_inst, IFETCH2, DECODE );

  cout << "IFETCH2 test passed" << endl; 

  cout << "-------------------------" << endl;
  cout << "Instruction Decode Cycle" << endl;


  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.out_enable_instr.read() != 0 ||
      tb_controller_inst.out_enable_a.read() != 1 ||
      tb_controller_inst.out_enable_a.read() != 1 ){

    cout << "Controller DECODE outputs failed" << endl;
  }
  else{
    cout << "Controller DECODE outputs passed" << endl;
  }

  state_test( tb_controller_inst, DECODE, EX_LD );

  cout << "Instruction DECODE cycle passed." << endl;

  cout << "-------------------------" << endl;
  cout << "Execution Cycle" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.out_alu_op.read() != 8 ||
      tb_controller_inst.out_enable_alu_result.read() != 1 ){

    cout << "Controller EX_LD outputs failed" << endl;
  }
  else{
    cout << "Controller EX_LD outputs passed" << endl;
  }


  state_test( tb_controller_inst, EX_LD, MEM_LD );

  cout << "EX_LD cycle passed." << endl;

  cout << "-------------------------" << endl;
  cout << "Load/Store Cycle - MEM_LD" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);


  if( tb_controller_inst.out_s_addr.read() != true ){
    cout << "Controller MEM_LD outputs failed" << endl;
  }
  else{
    cout << "Controller MEM_LD outputs passed" << endl;
  }

  state_test( tb_controller_inst, MEM_LD, MEM_LD2 );

  cout << "MEM_LD cycle passed." << endl;

  cout << "-------------------------" << endl;
  cout << "Load/Store Cycle - MEM_LD2" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.out_enable_mdr.read() != true ){
    cout << "Controller MEM_LD2 outputs failed" << endl;
  }
  else{
    cout << "Controller MEM_LD2 outputs passed" << endl;
  }

  state_test( tb_controller_inst, MEM_LD2, WB_LD );

  cout << "MEM_LD2 cycle passed." << endl;

  cout << "-------------------------" << endl;
  cout << "Load/Store Cycle - WB_LD" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  // Test the controller output signals
  if(tb_controller_inst.out_s_regfile_rw.read() != false ||
     tb_controller_inst.out_s_regfile_din.read() != true ||
     tb_controller_inst.out_reg_write.read() != true ||
     tb_controller_inst.out_alu_op.read() != 10 ||
     tb_controller_inst.out_enable_pc.read() != true ){

    cout << "WB_ALU signals Failed" << endl;

  }
  else{
    cout << "WB_ALU signals Passed" << endl;
  }

  state_test( tb_controller_inst, WB_LD, IFETCH );

  cout << "WB_LD cycle passed." << endl;

  cout << "-------------------------" << endl;
  cout << "-------------------------" << endl;
  cout << "Testing 0x8113 - ld $r1 $r3 1" << endl;
 
  cout << "-------------------------" << endl;
  cout << "Instruction Fetch Cycle IFETCH" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  if(tb_controller_inst.out_s_addr.read() != 0){

    cout << "Unexpected s_addr value of " << tb_controller_inst.out_s_addr.read() << endl;
    cout << "Start Test Failed" << endl;
    sc_stop();
    exit(-1);
  }
  else{
    cout << "Expected s_addr value of " << tb_controller_inst.out_s_addr.read() << endl;
  }

  cout << "-------------------------" << endl;
  cout << "Instruction Fetch Cycle IFETCH2" << endl;

  // Datapath is sending an opcode of 8 for LD
  tb_controller_inst.in_opcode.write(8);

  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.controller_dut.opcode_reg != 8 ){

    cout << "Unexpected result of opcode - " << tb_controller_inst.controller_dut.opcode_reg << endl;
    sc_stop();
    exit(-1);
  }
  else{
    cout << "Expected result of opcode - " << tb_controller_inst.controller_dut.opcode_reg << endl;
  }

  state_test( tb_controller_inst, IFETCH2, DECODE );

  cout << "IFETCH2 test passed" << endl; 

  cout << "-------------------------" << endl;
  cout << "Instruction Decode Cycle" << endl;


  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.out_enable_instr.read() != 0 ||
      tb_controller_inst.out_enable_a.read() != 1 ||
      tb_controller_inst.out_enable_a.read() != 1 ){

    cout << "Controller DECODE outputs failed" << endl;
  }
  else{
    cout << "Controller DECODE outputs passed" << endl;
  }

  state_test( tb_controller_inst, DECODE, EX_LD );

  cout << "Instruction DECODE cycle passed." << endl;

  cout << "-------------------------" << endl;
  cout << "Execution Cycle" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.out_alu_op.read() != 8 ||
      tb_controller_inst.out_enable_alu_result.read() != 1 ){

    cout << "Controller EX_LD outputs failed" << endl;
  }
  else{
    cout << "Controller EX_LD outputs passed" << endl;
  }


  state_test( tb_controller_inst, EX_LD, MEM_LD );

  cout << "EX_LD cycle passed." << endl;

  cout << "-------------------------" << endl;
  cout << "Load/Store Cycle - MEM_LD" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);


  if( tb_controller_inst.out_s_addr.read() != true ){
    cout << "Controller MEM_LD outputs failed" << endl;
  }
  else{
    cout << "Controller MEM_LD outputs passed" << endl;
  }

  state_test( tb_controller_inst, MEM_LD, MEM_LD2 );

  cout << "MEM_LD cycle passed." << endl;

  cout << "-------------------------" << endl;
  cout << "Load/Store Cycle - MEM_LD2" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.out_enable_mdr.read() != true ){
    cout << "Controller MEM_LD2 outputs failed" << endl;
  }
  else{
    cout << "Controller MEM_LD2 outputs passed" << endl;
  }

  state_test( tb_controller_inst, MEM_LD2, WB_LD );

  cout << "MEM_LD2 cycle passed." << endl;

  cout << "-------------------------" << endl;
  cout << "Load/Store Cycle - WB_LD" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  // Test the controller output signals
  if(tb_controller_inst.out_s_regfile_rw.read() != false ||
     tb_controller_inst.out_s_regfile_din.read() != true ||
     tb_controller_inst.out_reg_write.read() != true ||
     tb_controller_inst.out_alu_op.read() != 10 ||
     tb_controller_inst.out_enable_pc.read() != true ){

    cout << "WB_ALU signals Failed" << endl;

  }
  else{
    cout << "WB_ALU signals Passed" << endl;
  }

  state_test( tb_controller_inst, WB_LD, IFETCH );

  cout << "WB_LD cycle passed." << endl;

  cout << "-------------------------" << endl;
  cout << "-------------------------" << endl;
  cout << "Testing 0x0001 - add $r0 $r0 $r1" << endl;
  
  cout << "-------------------------" << endl;
  cout << "Instruction Fetch Cycle IFETCH" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  if(tb_controller_inst.out_s_addr.read() != 0){

    cout << "Unexpected s_addr value of " << tb_controller_inst.out_s_addr.read() << endl;
    cout << "Start Test Failed" << endl;
    sc_stop();
    exit(-1);
  }
  else{
    cout << "Expected s_addr value of " << tb_controller_inst.out_s_addr.read() << endl;
  }

  cout << "-------------------------" << endl;
  cout << "Instruction Fetch Cycle IFETCH2" << endl;

  // Datapath is sending an opcode of 0 for EX_ADD
  tb_controller_inst.in_opcode.write(0);

  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.controller_dut.opcode_reg != 0 ){

    cout << "Unexpected result of opcode - " << tb_controller_inst.controller_dut.opcode_reg << endl;
    sc_stop();
    exit(-1);
  }
  else{
    cout << "Expected result of opcode - " << tb_controller_inst.controller_dut.opcode_reg << endl;
  }

  state_test( tb_controller_inst, IFETCH2, DECODE );

  cout << "-------------------------" << endl;
  cout << "Instruction Decode Cycle" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.out_enable_instr.read() != 0 ||
      tb_controller_inst.out_enable_a.read() != 1 ||
      tb_controller_inst.out_enable_a.read() != 1 ){

    cout << "Controller DECODE outputs failed" << endl;
  }
  else{
    cout << "Controller DECODE outputs passed" << endl;
  }

  state_test( tb_controller_inst, DECODE, EX_ADD );

  cout << "Instruction DECODE cycle passed." << endl;

  cout << "IFETCH2 test passed" << endl; 

  cout << "-------------------------" << endl;
  cout << "Execution Cycle" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.out_alu_op.read() != 8 ||
      tb_controller_inst.out_enable_alu_result.read() != 1 ){

    cout << "Controller EX_ADD outputs failed" << endl;
  }
  else{
    cout << "Controller EX_ADD outputs passed" << endl;
  }


  state_test( tb_controller_inst, EX_ADD, WB_ALU );

  cout << "EX_ADD cycle passed." << endl;

  cout << "-------------------------" << endl;
  cout << "Load/Store Cycle - WB_ALU" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  // Test the controller output signals
  if(tb_controller_inst.out_s_regfile_rw.read() != 0 ||
     tb_controller_inst.out_s_regfile_din.read() != false ||
     tb_controller_inst.out_reg_write.read() != true ||
     tb_controller_inst.out_alu_op.read() != 10 ||
     tb_controller_inst.out_enable_pc.read() != true ){

    cout << "WB_ALU Failed" << endl;

  }
  else{
    cout << "WB_ALU Passed" << endl;
  }

  state_test( tb_controller_inst, WB_ALU, IFETCH );

  cout << "... Load/Store Test Passed!" << endl;


  cout << "-------------------------" << endl;
  cout << "-------------------------" << endl;
  cout << "Testing 0x9202 - st $0 2($2)" << endl;
  
  cout << "-------------------------" << endl;
  cout << "Instruction Fetch Cycle IFETCH" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);

  if(tb_controller_inst.out_s_addr.read() != 0){

    cout << "Unexpected s_addr value of " << tb_controller_inst.out_s_addr.read() << endl;
    cout << "Start Test Failed" << endl;
    sc_stop();
    exit(-1);
  }
  else{
    cout << "Expected s_addr value of " << tb_controller_inst.out_s_addr.read() << endl;
  }

  cout << "-------------------------" << endl;
  cout << "Instruction Fetch Cycle IFETCH2" << endl;

  // Datapath is sending an opcode of 9 for ST
  tb_controller_inst.in_opcode.write(9);

  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.controller_dut.opcode_reg != 9 ){

    cout << "Unexpected result of opcode - " << tb_controller_inst.controller_dut.opcode_reg << endl;
    sc_stop();
    exit(-1);
  }
  else{
    cout << "Expected result of opcode - " << tb_controller_inst.controller_dut.opcode_reg << endl;
  }

  state_test( tb_controller_inst, IFETCH2, DECODE );

  cout << "IFETCH2 test passed" << endl; 

  cout << "-------------------------" << endl;
  cout << "Instruction Decode Cycle" << endl;

 sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.out_enable_instr.read() != 0 ||
      tb_controller_inst.out_enable_a.read() != 1 ||
      tb_controller_inst.out_enable_a.read() != 1 ){

    cout << "Controller DECODE outputs failed" << endl;
  }
  else{
    cout << "Controller DECODE outputs passed" << endl;
  }

  state_test( tb_controller_inst, DECODE, EX_ST );

  cout << "Instruction DECODE cycle passed." << endl;

  cout << "-------------------------" << endl;
  cout << "Execution Cycle" << endl;


  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.out_alu_op.read() != 9 ||
      tb_controller_inst.out_enable_alu_result.read() != 1 ){

    cout << "Controller EX_ADD outputs failed" << endl;
  }
  else{
    cout << "Controller EX_ADD outputs passed" << endl;
  }


  state_test( tb_controller_inst, EX_ST, MEM_ST );

  cout << "EX_ST cycle passed." << endl;

  cout << "-------------------------" << endl;
  cout << "Load/Store Cycle - MEM_ST" << endl;

  sc_start(CYCLE_LENGTH,SC_NS);


  if( tb_controller_inst.out_we_mem.read() != true ||
      tb_controller_inst.out_s_addr.read() != true ||
      tb_controller_inst.out_alu_op.read() != 10 ||
      tb_controller_inst.out_enable_pc.read() != true
      ){
    cout << "Controller MEM_ST outputs failed" << endl;
  }
  else{
    cout << "Controller MEM_ST outputs passed" << endl;
  }

  state_test( tb_controller_inst, MEM_ST, IFETCH );

  cout << "MEM_ST cycle passed." << endl;

  cout << "-------------------------" << endl;
  cout << "-------------------------" << endl;
  cout << "Testing 0xF000 - quit" << endl;
  
  cout << "-------------------------" << endl;
  cout << "Instruction Fetch Cycle IFETCH" << endl;

  // Set start to 0 since the program has started
  tb_controller_inst.in_start.write(0);

  sc_start(CYCLE_LENGTH,SC_NS);

  if(tb_controller_inst.out_s_addr.read() != 0){

    cout << "Unexpected s_addr value of " << tb_controller_inst.out_s_addr.read() << endl;
    cout << "Start Test Failed" << endl;
    sc_stop();
    exit(-1);
  }
  else{
    cout << "Expected s_addr value of " << tb_controller_inst.out_s_addr.read() << endl;
  }


  cout << "-------------------------" << endl;
  cout << "Instruction Fetch Cycle IFETCH2" << endl;

  // Datapath is sending an opcode of 0xF for QUIT
  tb_controller_inst.in_opcode.write(0xF);

  sc_start(CYCLE_LENGTH,SC_NS);

  if( tb_controller_inst.controller_dut.opcode_reg != 0xF ){

    cout << "Unexpected result of opcode - " << tb_controller_inst.controller_dut.opcode_reg << endl;
    sc_stop();
    exit(-1);
  }
  else{
    cout << "Expected result of opcode - " << tb_controller_inst.controller_dut.opcode_reg << endl;
  }

  state_test( tb_controller_inst, IFETCH2, DECODE );

  cout << "IFETCH2 test passed" << endl; 

  cout << "-------------------------" << endl;


  // Do a reset by setting to false and running 1 cycle
  cout << "-------------------------" << endl;
  cout << "Running a reset test" << endl;
  cout << "The reset block will take 1 cycle" << endl;
  cout << "-------------------------" << endl;
  reset.write(false);
  tb_controller_inst.in_mem_we.write(0); 

  sc_start(CYCLE_LENGTH,SC_NS);

  // Test the Datapath Reset
  if(tb_controller_inst.test_reset_controller()){

    // If we got here, all the reset tests in tb_datapath passed
    cout << "Reset test PASSED" << endl;
  }
  else{
    cout << "Reset test FAILED" << endl;
    sc_stop();
    exit(EXIT_FAILURE);
  }


  // Conclude the test
  cout << "-------------------------" << endl;
  cout << "-------------------------" << endl;
  cout << "Controller Test completed successfully" << endl;
  cout << "-------------------------" << endl;
  cout << "-------------------------" << endl;
  sc_stop();

  return EXIT_SUCCESS;
};
