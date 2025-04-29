/*********************************************
 * File name: sc_controller.cpp
 * Author: Matthew Morrison
 * Date: 4/19/2025
 *
 * This file contains the sc_main function for
 * testing the albacore controller (refactored with step() method)
 *********************************************/

#include <systemc>
#include "controller.h"

// Helper function to check current and next state of controller
void state_test(const controller& controller_inst, const sc_bv<5>& expected_state, const sc_bv<5>& expected_next) {

    if (controller_inst.state != expected_state) {
        std::cout << "Unexpected state Register value of " << controller_inst.state << std::endl;
        std::cout << "State Test Failed" << std::endl;
        exit(EXIT_FAILURE);
    } 
    else {
        std::cout << "Expected state Register value of " << controller_inst.state << std::endl;
    }

    if (controller_inst.next_state != expected_next) {
        std::cout << "Unexpected next_state Register value of " << controller_inst.next_state << std::endl;
        std::cout << "State Test Failed" << std::endl;
        exit(EXIT_FAILURE);
    } 
    else {
        std::cout << "Expected next_state Register value of " << controller_inst.next_state << std::endl;
    }
	
    std::cout << "Start Test Passed" << std::endl;
}

void run_instruction_step( controller& controller_inst, sc_time& current_time, const sc_time& cycle_time, const std::string& detail, int n ){

  std::cout << detail << std::endl;
  std::cout << "Instruction cycle start time - " << current_time << std::endl;
  
  for(int i = 0; i < n; ++i) {
    sc_start(2*cycle_time);
    current_time += 2*cycle_time;
  }

}


int sc_main(int argc, char* argv[]) {

  // Create controller instance (plain C++ class)
  controller controller_inst("controller_inst");
  
  // Time and cycle management
  sc_time current_time = SC_ZERO_TIME;
  sc_time cycle_time(CYCLE_LENGTH, SC_NS);
  
  sc_clock clk("clk", CYCLE_LENGTH);
  sc_buffer<bool> reset;

  sc_buffer<bool> start;                      // Port 2
  sc_buffer<bool> mem_we;                     // Port 3 

  sc_buffer< sc_bv<4> > opcode;               // Port 4
  sc_buffer<bool> zero;                       // Port 5
  sc_buffer<bool> neg;                        // Port 6

  sc_buffer<bool> s_addr;                    // Port 7
  sc_buffer<bool> enable_instr;              // Port 8
  sc_buffer<bool> enable_a;                  // Port 9
  sc_buffer<bool> enable_b;                  // Port 10
  sc_buffer< sc_bv<ALU_OP_W> > alu_op;       // Port 11
  sc_buffer<bool> enable_alu_result;         // Port 12
  sc_buffer<bool> s_regfile_din;             // Port 13
  sc_buffer<bool> reg_write;                 // Port 14
  sc_buffer<bool> s_regfile_rw;              // Port 15
  sc_buffer<bool> enable_pc;                 // Port 16
  sc_buffer<bool> we_mem;                    // Port 17
  sc_buffer<bool> enable_mdr;                // Port 18
  
  controller_inst.clk(clk);
  controller_inst.reset(reset);

  controller_inst.start( start );                      // Port 2
  controller_inst.mem_we( mem_we );                     // Port 3 

  controller_inst.opcode( opcode );               // Port 4
  controller_inst.zero( zero );                       // Port 5
  controller_inst.neg( neg );                        // Port 6

  controller_inst.s_addr( s_addr );                    // Port 7
  controller_inst.enable_instr( enable_instr );              // Port 8
  controller_inst.enable_a( enable_a );                  // Port 9
  controller_inst.enable_b( enable_b );                  // Port 10
  controller_inst.alu_op( alu_op );       // Port 11
  controller_inst.enable_alu_result( enable_alu_result );         // Port 12
  controller_inst.s_regfile_din( s_regfile_din );             // Port 13
  controller_inst.reg_write( reg_write );                 // Port 14
  controller_inst.s_regfile_rw( s_regfile_rw );              // Port 15
  controller_inst.enable_pc( enable_pc );                 // Port 16
  controller_inst.we_mem( we_mem );                    // Port 17
  controller_inst.enable_mdr( enable_mdr );                // Port 18

  
  // Initialize signals
  std::cout << "----------------------------------------------" << std::endl;	
  std::cout << "Setting start to 0 and mem_we to 0 at " << current_time << std::endl;
  reset.write(true);
  start = false;
  mem_we = false;
  
    run_instruction_step(controller_inst,current_time,cycle_time,"Start up with 1 cycle",1);  // first cycle with reset high

    // Run reset sequence
    reset.write(false);
    mem_we = false;
	
	std::cout << "----------------------------------------------" << std::endl;
	
	run_instruction_step(controller_inst,current_time,cycle_time,"Reset test",1);

    // Test datapath reset behavior
    if (controller_inst.test_reset_controller()) {
        std::cout << "Reset test PASSED" << std::endl;
    } 
	else {
        std::cout << "Reset test FAILED" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "----------------------------------------------" << std::endl;

    // Simulate program loading into memory (8 cycles, mem_we true, start still 0)
	std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Eight cycles of memory writes (addresses 0-5 and 32-33)" << std::endl;
	std::cout << "----------------------------------------------" << std::endl;

    reset.write(true);
    mem_we = true;
    start = false;
	
    // 0x7220 written to Memory[0]
    run_instruction_step(controller_inst,current_time,cycle_time,"0x7220 written to Memory[0] - nothing in controller",1);

    // 0x8002 written to Memory[1]
    run_instruction_step(controller_inst,current_time,cycle_time,"0x8002 written to Memory[1] - nothing in controller",1);

    // 0x8112 written to Memory[2]
    run_instruction_step(controller_inst,current_time,cycle_time,"0x8112 written to Memory[2] - nothing in controller",1);
	
    // 0x0001 written to Memory[3]
    run_instruction_step(controller_inst,current_time,cycle_time,"0x0001 written to Memory[3] - nothing in controller",1);
	
    // 0x9202 written to Memory[4]
    run_instruction_step(controller_inst,current_time,cycle_time,"0x9202 written to Memory[4] - nothing in controller",1);
	
    // 0xF000 written to Memory[5]
    run_instruction_step(controller_inst,current_time,cycle_time,"0xF000 written to Memory[5] - nothing in controller",1);
	
    // 0x0003 written to Memory[32]
    run_instruction_step(controller_inst,current_time,cycle_time,"0x0003 written to Memory[32] - nothing in controller",1);
	
    // 0x0004 written to Memory[33]
    run_instruction_step(controller_inst,current_time,cycle_time,"0x0004 written to Memory[33] - nothing in controller",1);

    // Start the controller (start = 1 for one cycle)
	std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Clearing reset and starting datapath tests" << std::endl;
    std::cout << "This test includes the six instructions from PLPD pg. 296" << std::endl;
    std::cout << "A compiling for the following C code:" << std::endl;
    std::cout << "int main(){" << std::endl;
    std::cout << "    A[0] = 2;" << std::endl;
    std::cout << "    A[1] = 3;" << std::endl;
    std::cout << "    A[2] = A[1] + A[0];" << std::endl;
    std::cout << "    return 0;" << std::endl;
    std::cout << "}" << std::endl;
	std::cout << "----------------------------------------------" << std::endl;
	
    start = true;
    mem_we = false;
	
    run_instruction_step(controller_inst,current_time,cycle_time,"Testing Start Signal",1);
	
    // After start, controller state should be IFETCH, next_state IFETCH
    state_test(controller_inst, sc_bv<5>("00000"), sc_bv<5>("00000"));  // IFETCH assumed "00000"

    // ** Instruction 0x7220 (ldi) **
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Testing 0x7220 - ldi $2 32" << std::endl;

    start = false;  // program already started
    run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH",1);

    // Controller outputs for IFETCH: out_s_addr should be 0
    if (controller_inst.out_s_addr != false) {
        std::cout << "Unexpected s_addr value of " << controller_inst.out_s_addr << std::endl;
        return EXIT_FAILURE;
    } 
    else {
        std::cout << "Expected s_addr value of " << controller_inst.out_s_addr << std::endl;
    }
	
    state_test(controller_inst, IFETCH, IFETCH2);

    std::cout << "----------------------------------------------" << std::endl;
	
    run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH2",1);

    state_test(controller_inst, IFETCH2, DECODE);
	
    std::cout << "IFETCH2 test passed" << std::endl;

    std::cout << "----------------------------------------------" << std::endl;

    // Datapath sends opcode 7 (ldi) to controller
    opcode = sc_bv<4>(7);

    run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Decode Cycle",1);
	
    // Controller should latch opcode (opcode_reg == 7)
    if (controller_inst.opcode_reg != 7) {
        std::cout << "Unexpected result of opcode_reg - " << controller_inst.opcode_reg << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Expected result of opcode_reg - " << controller_inst.opcode_reg << std::endl;
    }
    
	
    // Controller outputs in DECODE for ldi: out_enable_instr=0, out_enable_a=1, out_enable_b=1
    if (controller_inst.out_enable_instr != 0 || controller_inst.out_enable_a != 1 || controller_inst.out_enable_b != 1) {
        std::cout << "Controller DECODE outputs failed" << std::endl;
    } 
	else {
        std::cout << "Controller DECODE outputs passed" << std::endl;
    }
	
    state_test(controller_inst, DECODE, EX_LDI);
	
    std::cout << "Instruction DECODE cycle passed." << std::endl;

    std::cout << "----------------------------------------------" << std::endl;
    
    run_instruction_step(controller_inst,current_time,cycle_time,"Execution Cycle",1);
	
    // Controller outputs in EX_LDI: out_alu_op = 7, out_enable_alu_result = 1
    if (controller_inst.out_alu_op != sc_bv<ALU_OP_W>(7) || controller_inst.out_enable_alu_result != true) {
        std::cout << "Controller EX_LDI outputs failed" << std::endl;
    } 
	else {
        std::cout << "Controller EX_LDI outputs passed" << std::endl;
    }
	
    state_test(controller_inst, EX_LDI, WB_ALU);
	
    std::cout << "EX_LDI cycle passed." << std::endl;

    std::cout << "----------------------------------------------" << std::endl;
    
    run_instruction_step(controller_inst,current_time,cycle_time,"Load/Store Cycle - WB_ALU",1);
	
    // Controller outputs in WB_ALU: s_regfile_rw=0, s_regfile_din=0, reg_write=1, alu_op=10, enable_pc=1
    if (controller_inst.out_s_regfile_rw != false || controller_inst.out_s_regfile_din != false ||
        controller_inst.out_reg_write != true || controller_inst.out_alu_op != sc_bv<ALU_OP_W>(10) ||
        controller_inst.out_enable_pc != true) {
        std::cout << "WB_ALU signals Failed" << std::endl;
    } 
	else {
        std::cout << "WB_ALU signals Passed" << std::endl;
    }
	
    state_test(controller_inst, WB_ALU, IFETCH);
	
    std::cout << "WB_ALU Passed" << std::endl;
	
    std::cout << "... Write Back Test Passed!" << std::endl;

    // ** Instruction 0x8002 (ld) **
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Testing 0x8002 - ld $r0 $r2 0" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH",1);
	
    // s_addr should remain 0
    if (controller_inst.out_s_addr != 0) {
        std::cout << "Unexpected s_addr value of " << controller_inst.out_s_addr << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Expected s_addr value of " << controller_inst.out_s_addr << std::endl;
    }
	
    state_test(controller_inst, IFETCH, IFETCH2);
	
    std::cout << "IFETCH Passed" << std::endl;
	
    std::cout << "----------------------------------------------" << std::endl;

    // Datapath sends opcode 8 (ld)

	
    run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH2",1);
	

	
    state_test(controller_inst, IFETCH2, DECODE);
	
    std::cout << "IFETCH2 test passed" << std::endl;

    std::cout << "----------------------------------------------" << std::endl;

    opcode = sc_bv<4>(8);
    
    run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Decode Cycle",1);

    if (controller_inst.opcode_reg != 8) {
        std::cout << "Unexpected result of opcode_reg - " << controller_inst.opcode_reg << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Expected result of opcode_reg - " << controller_inst.opcode_reg << std::endl;
    }
	
    if (controller_inst.out_enable_instr != 0 || controller_inst.out_enable_a != 1 || controller_inst.out_enable_b != 1) {
        std::cout << "Controller DECODE outputs failed" << std::endl;
    } 
	else {
        std::cout << "Controller DECODE outputs passed" << std::endl;
    }
	
    state_test(controller_inst, DECODE, EX_LD);
	
    std::cout << "Instruction DECODE cycle passed." << std::endl;

    std::cout << "----------------------------------------------" << std::endl;
    
    run_instruction_step(controller_inst,current_time,cycle_time,"Execution Cycle",1);
	
    if (controller_inst.out_alu_op != sc_bv<ALU_OP_W>(8) || controller_inst.out_enable_alu_result != 1) {
        std::cout << "Controller EX_LD outputs failed" << std::endl;
    } 
	else {
        std::cout << "Controller EX_LD outputs passed" << std::endl;
    }
	
    state_test(controller_inst, EX_LD, MEM_LD);
	
    std::cout << "EX_LD cycle passed." << std::endl;

    std::cout << "----------------------------------------------" << std::endl;
    
    run_instruction_step(controller_inst,current_time,cycle_time,"Load/Store Cycle - MEM_LD",1);
	
    if (controller_inst.out_s_addr != true) {
        std::cout << "Controller MEM_LD outputs failed" << std::endl;
    } 
	else {
        std::cout << "Controller MEM_LD outputs passed" << std::endl;
    }
	
    state_test(controller_inst, MEM_LD, MEM_LD2);
	
    std::cout << "MEM_LD cycle passed." << std::endl;

    std::cout << "----------------------------------------------" << std::endl;
    
    run_instruction_step(controller_inst,current_time,cycle_time,"Load/Store Cycle - MEM_LD2",1);
	
    if (controller_inst.out_enable_mdr != true) {
        std::cout << "Controller MEM_LD2 outputs failed" << std::endl;
    } 
	else {
        std::cout << "Controller MEM_LD2 outputs passed" << std::endl;
    }
    state_test(controller_inst, MEM_LD2, WB_LD);
    std::cout << "MEM_LD2 cycle passed." << std::endl;

    std::cout << "----------------------------------------------" << std::endl;
    
    run_instruction_step(controller_inst,current_time,cycle_time,"Load/Store Cycle - WB_LD",1);
	
    // WB_LD outputs: s_regfile_rw=0, s_regfile_din=1, reg_write=1, alu_op=10, enable_pc=1
    if (controller_inst.out_s_regfile_rw != false || controller_inst.out_s_regfile_din != true ||
        controller_inst.out_reg_write != true || controller_inst.out_alu_op != sc_bv<ALU_OP_W>(10) ||
        controller_inst.out_enable_pc != true) {
        std::cout << "WB_LD signals Failed" << std::endl;
    } 
	else {
        std::cout << "WB_LD signals Passed" << std::endl;
    }
    state_test(controller_inst, WB_LD, IFETCH);
	
    std::cout << "WB_LD cycle passed." << std::endl;
	
    std::cout << "... Write Back Test Passed!" << std::endl;


    // ** Instruction 0x8113 (ld) **
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Testing 0x8112 - ld $r1 $r2 1" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH",1);
	
    // s_addr should remain 0
    if (controller_inst.out_s_addr != 0) {
        std::cout << "Unexpected s_addr value of " << controller_inst.out_s_addr << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Expected s_addr value of " << controller_inst.out_s_addr << std::endl;
    }
	
    state_test(controller_inst, IFETCH, IFETCH2);
	
    std::cout << "IFETCH Passed" << std::endl;
	
    std::cout << "----------------------------------------------" << std::endl;

    // Datapath sends opcode 8 (ld)

	
    run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH2",1);
	

	
    state_test(controller_inst, IFETCH2, DECODE);
	
    std::cout << "IFETCH2 test passed" << std::endl;

    std::cout << "----------------------------------------------" << std::endl;

    opcode = sc_bv<4>(8);
    
    run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Decode Cycle",1);

    if (controller_inst.opcode_reg != 8) {
        std::cout << "Unexpected result of opcode_reg - " << controller_inst.opcode_reg << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Expected result of opcode_reg - " << controller_inst.opcode_reg << std::endl;
    }
	
    if (controller_inst.out_enable_instr != 0 || controller_inst.out_enable_a != 1 || controller_inst.out_enable_b != 1) {
        std::cout << "Controller DECODE outputs failed" << std::endl;
    } 
	else {
        std::cout << "Controller DECODE outputs passed" << std::endl;
    }
	
    state_test(controller_inst, DECODE, EX_LD);
	
    std::cout << "Instruction DECODE cycle passed." << std::endl;

    std::cout << "----------------------------------------------" << std::endl;
    
    run_instruction_step(controller_inst,current_time,cycle_time,"Execution Cycle",1);
	
    if (controller_inst.out_alu_op != sc_bv<ALU_OP_W>(8) || controller_inst.out_enable_alu_result != 1) {
        std::cout << "Controller EX_LD outputs failed" << std::endl;
    } 
	else {
        std::cout << "Controller EX_LD outputs passed" << std::endl;
    }
	
    state_test(controller_inst, EX_LD, MEM_LD);
	
    std::cout << "EX_LD cycle passed." << std::endl;

    std::cout << "----------------------------------------------" << std::endl;
    
    run_instruction_step(controller_inst,current_time,cycle_time,"Load/Store Cycle - MEM_LD",1);
	
    if (controller_inst.out_s_addr != true) {
        std::cout << "Controller MEM_LD outputs failed" << std::endl;
    } 
	else {
        std::cout << "Controller MEM_LD outputs passed" << std::endl;
    }
	
    state_test(controller_inst, MEM_LD, MEM_LD2);
	
    std::cout << "MEM_LD cycle passed." << std::endl;

    std::cout << "----------------------------------------------" << std::endl;
    
    run_instruction_step(controller_inst,current_time,cycle_time,"Load/Store Cycle - MEM_LD2",1);
	
    if (controller_inst.out_enable_mdr != true) {
        std::cout << "Controller MEM_LD2 outputs failed" << std::endl;
    } 
	else {
        std::cout << "Controller MEM_LD2 outputs passed" << std::endl;
    }
    state_test(controller_inst, MEM_LD2, WB_LD);
    std::cout << "MEM_LD2 cycle passed." << std::endl;

    std::cout << "----------------------------------------------" << std::endl;
    
    run_instruction_step(controller_inst,current_time,cycle_time,"Load/Store Cycle - WB_LD",1);
	
    // WB_LD outputs: s_regfile_rw=0, s_regfile_din=1, reg_write=1, alu_op=10, enable_pc=1
    if (controller_inst.out_s_regfile_rw != false || controller_inst.out_s_regfile_din != true ||
        controller_inst.out_reg_write != true || controller_inst.out_alu_op != sc_bv<ALU_OP_W>(10) ||
        controller_inst.out_enable_pc != true) {
        std::cout << "WB_LD signals Failed" << std::endl;
    } 
	else {
        std::cout << "WB_LD signals Passed" << std::endl;
    }
    state_test(controller_inst, WB_LD, IFETCH);
	
    std::cout << "WB_LD cycle passed." << std::endl;
	
    std::cout << "... Write Back Test Passed!" << std::endl;
	
	
    // ** Instruction 0x0001 (add $r0, $r0, $r1) **
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Testing 0x0001 - add $r0 $r0 $r1" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;


    run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH",1);
	
    // s_addr should remain 0
    if (controller_inst.out_s_addr != 0) {
        std::cout << "Unexpected s_addr value of " << controller_inst.out_s_addr << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Expected s_addr value of " << controller_inst.out_s_addr << std::endl;
    }
	
    state_test(controller_inst, IFETCH, IFETCH2);
	
    std::cout << "IFETCH Passed" << std::endl;
	
    std::cout << "----------------------------------------------" << std::endl;

    // Datapath sends opcode 0 (add)
   
 
    run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH2",1);

	
    state_test(controller_inst, IFETCH2, DECODE);
	
    std::cout << "IFETCH2 test passed" << std::endl;

    std::cout << "----------------------------------------------" << std::endl;

   opcode = sc_bv<4>(0);
	
    run_instruction_step(controller_inst,current_time,cycle_time,"DECODE",1);
	
    if (controller_inst.opcode_reg != 0) {
        std::cout << "Unexpected result of opcode_reg - " << controller_inst.opcode_reg << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Expected result of opcode_reg - " << controller_inst.opcode_reg << std::endl;
    }


    
    if( controller_inst.out_enable_instr != 0 ||
	controller_inst.out_enable_a != 1 ||
	controller_inst.out_enable_a != 1 ){
      
      cout << "Controller DECODE outputs failed" << endl;
    }
    else{
      cout << "Controller DECODE outputs passed" << endl;
    }
    
    state_test(controller_inst, DECODE, EX_ADD);
    
    std::cout << "DECODE test passed" << std::endl;
    
    std::cout << "-------------------------" << std::endl;
    run_instruction_step(controller_inst,current_time,cycle_time,"Execution Cycle",1);
    
    if( controller_inst.out_alu_op != 8 ||
	controller_inst.out_enable_alu_result != 1 ){
      
      std::cout << "Controller EX_ADD outputs failed" << std::endl;
    }
    else{
      std::cout << "Controller EX_ADD outputs passed" << std::endl;
    }
    
    
    state_test( controller_inst, EX_ADD, WB_ALU );
    
    std::cout << "EX_ADD cycle passed." << std::endl;
    
    std::cout << "-------------------------" << std::endl;
    
    run_instruction_step(controller_inst,current_time,cycle_time,"Load/Store Cycle - WB_ALU",1);
    
    // Test the controller output signals
    if(controller_inst.out_s_regfile_rw != 0 ||
       controller_inst.out_s_regfile_din != false ||
       controller_inst.out_reg_write != true ||
       controller_inst.out_alu_op != 10 ||
       controller_inst.out_enable_pc != true ){
      
      std::cout << "WB_ALU Failed" << std::endl;
      
    }
    else{
      std::cout << "WB_ALU Passed" << std::endl;
    }
    
    state_test( controller_inst, WB_ALU, IFETCH );
    
    std::cout << "... Write Back Test Passed!" << std::endl;
    
    
    std::cout << "-------------------------" << std::endl;
    std::cout << "-------------------------" << std::endl;
  std::cout << "Testing 0x9202 - st $0 2($2)" << std::endl;
  
  std::cout << "-------------------------" << std::endl;
  
  run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH",1);
  
  if(controller_inst.out_s_addr != 0){

    std::cout << "Unexpected s_addr value of " << controller_inst.out_s_addr << std::endl;
    std::cout << "Start Test Failed" << std::endl;
    sc_stop();
    exit(-1);
  }
  else{
    std::cout << "Expected s_addr value of " << controller_inst.out_s_addr << std::endl;
  }

  std::cout << "-------------------------" << std::endl;

  // Datapath is sending an opcode of 9 for ST


  run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH2",1);



  state_test( controller_inst, IFETCH2, DECODE );

  std::cout << "IFETCH2 test passed" << std::endl; 

  std::cout << "-------------------------" << std::endl;
  std::cout << "Instruction Decode Cycle" << std::endl;

  opcode = sc_bv<4>(9);

  run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Decode Cycle",1);

  if( controller_inst.opcode_reg != 9 ){

    std::cout << "Unexpected result of opcode - " << controller_inst.opcode_reg << std::endl;
    sc_stop();
    exit(-1);
  }
  else{
    std::cout << "Expected result of opcode - " << controller_inst.opcode_reg << std::endl;
  }

  if( controller_inst.out_enable_instr != 0 ||
      controller_inst.out_enable_a != 1 ||
      controller_inst.out_enable_a != 1 ){

    std::cout << "Controller DECODE outputs failed" << std::endl;
  }
  else{
    std::cout << "Controller DECODE outputs passed" << std::endl;
  }

  state_test( controller_inst, DECODE, EX_ST );

  std::cout << "Instruction DECODE cycle passed." << std::endl;

  std::cout << "-------------------------" << std::endl;

  run_instruction_step(controller_inst,current_time,cycle_time,"Execution Cycle",1);

  if( controller_inst.out_alu_op != 9 ||
      controller_inst.out_enable_alu_result != 1 ){
    std::cout << "Controller EX_ADD outputs failed" << std::endl;
  }
  else{
    std::cout << "Controller EX_ADD outputs passed" << std::endl;
  }

  state_test( controller_inst, EX_ST, MEM_ST );

  std::cout << "EX_ST cycle passed." << std::endl;

  std::cout << "-------------------------" << std::endl;
  std::cout << "Load/Store Cycle - MEM_ST" << std::endl;

  run_instruction_step(controller_inst,current_time,cycle_time,"Load/Store Cycle - MEM_ST",1);

  if( controller_inst.out_we_mem != true ||
      controller_inst.out_s_addr != true ||
      controller_inst.out_alu_op != 10 ||
      controller_inst.out_enable_pc != true
      ){
    std::cout << "Controller MEM_ST outputs failed" << std::endl;
  }
  else{
    std::cout << "Controller MEM_ST outputs passed" << std::endl;
  }

  state_test( controller_inst, MEM_ST, IFETCH );

  std::cout << "MEM_ST cycle passed." << std::endl;

  std::cout << "-------------------------" << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "Testing 0xF000 - quit" << std::endl;
  
  std::cout << "-------------------------" << std::endl;

  run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH",1);

  if(controller_inst.out_s_addr != 0){

    std::cout << "Unexpected s_addr value of " << controller_inst.out_s_addr << std::endl;
    std::cout << "Start Test Failed" << std::endl;
    sc_stop();
    exit(-1);
  }
  else{
    std::cout << "Expected s_addr value of " << controller_inst.out_s_addr << std::endl;
  }


  std::cout << "-------------------------" << std::endl;

  run_instruction_step(controller_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH2",1);

  state_test( controller_inst, IFETCH2, DECODE );

  std::cout << "IFETCH2 test passed" << std::endl; 

  std::cout << "-------------------------" << std::endl;

  // Datapath is sending an opcode of 0xF for QUIT
  opcode = sc_bv<4>(0xF);

  run_instruction_step(controller_inst,current_time,cycle_time,"DECODE",1);

  if( controller_inst.opcode_reg != 0xF ){

    std::cout << "Unexpected result of opcode - " << controller_inst.opcode_reg << std::endl;
    sc_stop();
    exit(-1);
  }
  else{
    std::cout << "Expected result of opcode - " << controller_inst.opcode_reg << std::endl;
  }
	

    // Final reset test at end of program
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Running a reset test" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
	
    reset.write(false);
    run_instruction_step(controller_inst,current_time,cycle_time,"Reset test",1);


    if (controller_inst.test_reset_controller()) {
      std::cout << "Reset test PASSED" << std::endl;
    } 
    else {
      std::cout << "Reset test FAILED" << std::endl;
      return EXIT_FAILURE;
    }

    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Controller Test completed successfully" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    return EXIT_SUCCESS;
}
