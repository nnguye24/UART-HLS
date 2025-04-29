/*********************************************
 * File name: sc_datapath.cpp
 * Author: Matthew Morrison
 * Date: 4/19/2025
 *
 * This file contains the sc_main function for
 * testing the albacore datapath (refactored with step() method)
 *********************************************/

#include "../src/datapath.h"

void run_instruction_step( datapath& datapath_inst, sc_time& current_time, const sc_time& cycle_time, const std::string& detail, int num_cycles ){

  std::cout << detail << endl;
  std::cout << "Instruction cycle start time - " << current_time << std::endl;
  
  for(int i = 0; i < num_cycles; ++i) {
    sc_start(2*cycle_time);
    current_time += 2*cycle_time;
  }

}

int sc_main(int argc, char* argv[]) {

  // Create the Datapath instance (plain C++ class with step method)
  datapath datapath_inst("datapath_inst");
  
  // Local variables to emulate clock cycles and signals
  sc_time current_time = SC_ZERO_TIME;
  sc_time cycle_time(CYCLE_LENGTH, SC_NS);

  sc_clock clk("clk", CYCLE_LENGTH);
  sc_buffer<bool> reset;
  
  datapath_inst.clk(clk);
  datapath_inst.reset(reset);
  
  // Set inputs and outputs from the datapath
  sc_buffer<bool> start;
  sc_buffer<bool> mem_we;
  sc_buffer< sc_bv<ALU_OP_W> >  alu_op;
  sc_buffer< sc_bv<DATA_W> > data_in;
  sc_buffer<bool> enable_instruction;
  sc_buffer<bool>  enable_a;
  sc_buffer<bool>  enable_b;
  sc_buffer<bool>  enable_pc;
  sc_buffer<bool>  enable_alu_result;
  sc_buffer<bool>  enable_mdr;
  sc_buffer<bool>  s_regfile_rw;
  sc_buffer<bool>  s_regfile_din;
  sc_buffer<bool>  reg_write;
  sc_buffer<bool>  s_addr;

  sc_buffer< sc_bv<ALU_OP_W> > out_opcode;
  sc_buffer< sc_bv<DATA_W> > out_data_out;
  sc_buffer< sc_bv<ADDR_W> > out_addr_out;
  sc_buffer<bool> out_zero;
  sc_buffer<bool> out_neg;

  // Connect the signals to the datapath_inst
  datapath_inst.start(start);
  datapath_inst.mem_we( mem_we );
  datapath_inst.alu_op( alu_op );
  datapath_inst.data_in( data_in );
  datapath_inst.enable_instruction( enable_instruction );
  datapath_inst.enable_a( enable_a );
  datapath_inst.enable_b( enable_b );
  datapath_inst.enable_pc( enable_pc );
  datapath_inst.enable_alu_result( enable_alu_result );
  datapath_inst.enable_mdr( enable_mdr );
  datapath_inst.s_regfile_rw( s_regfile_rw );
  datapath_inst.s_regfile_din( s_regfile_din );
  datapath_inst.reg_write( reg_write );
  datapath_inst.s_addr( s_addr );

  datapath_inst.opcode( out_opcode );
  datapath_inst.data_out( out_data_out );
  datapath_inst.addr_out( out_addr_out );
  datapath_inst.zero( out_zero );
  datapath_inst.neg( out_neg );

    // Set initial signals
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Setting start to 0 and mem_we to 0 at " << current_time << std::endl;

    // Startup with 1 cycle
    start = false;
    mem_we = false;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Start up with 1 cycle", 1);
	
    // Run reset sequence
    reset.write(false);
    mem_we = false;
	std::cout << "----------------------------------------------" << std::endl;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Run reset test", 1);

    // Test datapath reset behavior
    if (datapath_inst.test_reset_datapath()) {
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
    run_instruction_step(datapath_inst,current_time,cycle_time,"0x7220 written to Memory[0] - nothing in datapath",1);

    // 0x8002 written to Memory[1]
    run_instruction_step(datapath_inst,current_time,cycle_time,"0x8002 written to Memory[1] - nothing in datapath",1);

    // 0x8112 written to Memory[2]
    run_instruction_step(datapath_inst,current_time,cycle_time,"0x8112 written to Memory[2] - nothing in datapath",1);
	
    // 0x0001 written to Memory[3]
    run_instruction_step(datapath_inst,current_time,cycle_time,"0x0001 written to Memory[3] - nothing in datapath",1);
	
    // 0x9202 written to Memory[4]
    run_instruction_step(datapath_inst,current_time,cycle_time,"0x9202 written to Memory[4] - nothing in datapath",1);
	
    // 0xF000 written to Memory[5]
    run_instruction_step(datapath_inst,current_time,cycle_time,"0xF000 written to Memory[5] - nothing in datapath",1);
	
    // 0x0003 written to Memory[32]
    run_instruction_step(datapath_inst,current_time,cycle_time,"0x0003 written to Memory[32] - nothing in datapath",1);
	
    // 0x0004 written to Memory[33]
    run_instruction_step(datapath_inst,current_time,cycle_time,"0x0004 written to Memory[33] - nothing in datapath",1);

    // Now start the actual datapath operation tests
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

    // Set start signal to begin execution
    start = true;
    mem_we = false;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Testing Start Signal",1);
	
    // After starting, PC should remain 0 (starting address)
    if (datapath_inst.pc_reg != 0) {
        std::cout << "Unexpected PC Register value of " << datapath_inst.pc_reg << std::endl;
        std::cout << "Start Test Failed" << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Expected PC Register value of " << datapath_inst.pc_reg << std::endl;
    }
	
    std::cout << "Start Test Passed" << std::endl;

    // ** Instruction 0x7220 (ldi $2, 32) **
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Testing 0x7220 - ldi $2 32" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    
    // Set start to 0 (program already started), select PC for address
    start = false;
    s_addr = false;  // 0 selects PC for address output
    
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH",1);
    
    // PC was reset to 0, expecting out_addr_out = 0
    if (out_addr_out.read() != 0) {
      std::cout << "PC Register expected 0, got " << datapath_inst.out_addr_out << std::endl;
      return EXIT_FAILURE;
    } 
    else {
      std::cout << "Got expected PC Address of " << datapath_inst.out_addr_out << "... IFETCH test passed" << std::endl;
    }
    
    std::cout << "----------------------------------------------" << std::endl;
    
    // Provide the instruction data (0x7220) on data_in and enable instruction register
    enable_instruction = true;
    data_in = sc_bv<DATA_W>(0x7220);
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH2",1);
	
    // Opcode should be 7 (ldi)
    if(datapath_inst.instr_reg.range(15,12) != 7) {
        std::cout << "Opcode for ldi expected 7, got " << datapath_inst.out_opcode << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Got expected opcode of " << datapath_inst.out_opcode << "... IFETCH2 test passed" << std::endl;
    }

    std::cout << "----------------------------------------------" << std::endl;
	
    // Latch instruction into decode registers, enable A and B registers to load operands
    enable_instruction = false;
    enable_a = true;
    enable_b = true;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Decode Cycle",1);
	
    // For ldi, expecting ALU A and B registers to be 0 (no source operands, immediate handled separately)
    if (datapath_inst.alu_a_reg == 0 && datapath_inst.alu_b_reg == 0) {
        std::cout << "Got expected values of " << datapath_inst.alu_a_reg << " and " << datapath_inst.alu_b_reg << std::endl;
        std::cout << "Instruction Decode PASSED" << std::endl;
    } 
	else {
        std::cout << "Got unexpected values of " << datapath_inst.alu_a_reg << " and " << datapath_inst.alu_b_reg << std::endl;
        std::cout << "Instruction Decode Failed" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "----------------------------------------------" << std::endl;
	
    // Controller would set ALU operation for ldi and enable ALU result
    enable_a = false;
    enable_b = false;
    alu_op = sc_bv<ALU_OP_W>(7);
    enable_alu_result = true;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Execute Cycle",1);
	
    // ALU result register should contain the immediate value 0x0020 (32)
    if (datapath_inst.alu_result_reg == 0x0020) {
        std::cout << "Got expected alu_result reg of " << datapath_inst.alu_result_reg << std::endl;
    } 
	else {
        std::cout << "The Alu Result Register value is " << datapath_inst.alu_result_reg << "... Execution Cycle Test Failed" << std::endl;
        return EXIT_FAILURE;
    }
	
    std::cout << "... Execution Stage Test Passed!" << std::endl;

    std::cout << "----------------------------------------------" << std::endl;
	
    // Write-back stage for ldi: write ALU result to register file and increment PC
    enable_alu_result = true;  // keep ALU result enabled for write-back
    s_regfile_rw = false;
    s_regfile_din = false;
    reg_write = true;
    alu_op = sc_bv<ALU_OP_W>(10);  // ALU opcode 10 for PC = PC + 1
    enable_pc = true;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Write Back Cycle - WB_ALU",1);
	
    // The decode stage captured destination register index (should be 2 for $r2)

	
    // Register $2 should now contain 0x0020
    if (datapath_inst.register_file[2] == 0x0020) {
        std::cout << "Got expected register_file[2] = " << datapath_inst.register_file[2] << std::endl;
    } 
	else {
        std::cout << "Got unexpected register_file[2] = " << datapath_inst.register_file[2] << std::endl;
        std::cout << "Load/Store Test Failed" << std::endl;
        return EXIT_FAILURE;
    }
	
    // PC was incremented by 1, alu_result_reg holds PC+1 = 1
    if (datapath_inst.alu_result_reg == 1) {
        std::cout << "Got expected alu_result reg of " << datapath_inst.alu_result_reg << std::endl;
    } 
	else {
        std::cout << "The Alu Result Register value is " << datapath_inst.alu_result_reg << "... Load/Store Test Failed" << std::endl;
        return EXIT_FAILURE;
    }
	
    std::cout << "... Load/Store Test Passed!" << std::endl;

    // ** Instruction 0x8002 (ld $r0, $r2, 0) **
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Testing 0x8002 - ld $r0 $r2 0" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    // Prepare for next instruction fetch: PC should be 1
    enable_pc = false;
    reg_write = false;
    alu_op = sc_bv<ALU_OP_W>(0);
    s_addr = false;  // select PC for address
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH",1);
	
    // Expect PC = 1 -> out_addr_out = 1
    if(out_addr_out.read() != 1) {
        std::cout << "PC Register expected 1, got " << datapath_inst.out_addr_out << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Got expected PC Address of " << datapath_inst.out_addr_out << "... IFETCH test passed" << std::endl;
    }

    std::cout << "----------------------------------------------" << std::endl;
	
    // Provide instruction 0x8002 on data_in and enable instruction latch
    enable_instruction = true;
    data_in = sc_bv<DATA_W>(0x8002);
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH2",1);
	
    // Opcode should be 8 (ld)
    if(out_opcode.read() != 8) {
        std::cout << "Opcode for ld expected 8, got " << datapath_inst.out_opcode << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Got expected opcode of " << datapath_inst.out_opcode << "... IFETCH2 test passed" << std::endl;
    }

    std::cout << "----------------------------------------------" << std::endl;
	
    // Decode stage: latch operands
    enable_instruction = false;
    enable_a = true;
    enable_b = true;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Decode Cycle",1);
	
    // Expecting ALU A and B registers to contain $r0 and $r2 values (r0 was 0, r2 is 32)
    if (datapath_inst.alu_a_reg == 0 && datapath_inst.alu_b_reg == 32) {
        std::cout << "Got expected values of " << datapath_inst.alu_a_reg << " and " << datapath_inst.alu_b_reg << std::endl;
        std::cout << "Instruction Decode PASSED" << std::endl;
    } 
	else {
        std::cout << "Got unexpected values of " << datapath_inst.alu_a_reg << " and " << datapath_inst.alu_b_reg << std::endl;
        std::cout << "Instruction Decode Failed" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "----------------------------------------------" << std::endl;
	
    // Execute stage for ld: controller sets ALU op = 8, enable ALU result
    enable_a = false;
    enable_b = false;
    alu_op = sc_bv<ALU_OP_W>(8);
    enable_alu_result = true;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Execution Cycle",1);
	
    // ALU result = base address + offset = 0x0020 (32) + 0 = 0x0020
    if (datapath_inst.alu_result_reg == 0x0020) {
        std::cout << "Got expected alu_result reg of " << datapath_inst.alu_result_reg << std::endl;
        std::cout << "... Execution Stage Test Passed!" << std::endl;
    } 
	else {
        std::cout << "The Alu Result Register value is " << datapath_inst.alu_result_reg << "... Execution Cycle Test Failed" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "----------------------------------------------" << std::endl;

    // Memory stage: output memory address on addr_out
    enable_alu_result = false;
    s_addr = true;  // 1 selects ALU result (memory address) for addr_out
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Load/Store Cycle - MEM_LD",1);
	
    // out_addr_out should be 32 (the memory address to load from)
    if(out_addr_out.read() == 32) {
        std::cout << "Got expected addr_out = " << datapath_inst.out_addr_out << std::endl;
        std::cout << "... MEM_LD Stage Test Passed!" << std::endl;
    } 
	else {
        std::cout << "The addr_out value is " << datapath_inst.out_addr_out << "... MEM_LD Test Failed" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "----------------------------------------------" << std::endl;

    // Memory stage 2: latch data from memory into MDR
    s_addr = false;  // switch address selector back (not used further this cycle)
    enable_mdr = true;
	
    // Provide the data from memory location 32 (expected 0x0003)
    data_in = 0x0003;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Load/Store Cycle - MEM_LD2",1);
	
    // mdr_reg should contain 0x0003
    if (datapath_inst.mdr_reg == 0x0003) {
        std::cout << "The mdr_reg contains the expected value of " << datapath_inst.mdr_reg << std::endl;
        std::cout << "MEM_LD2 Test Passed" << std::endl;
    } 
	else {
        std::cout << "The mdr_reg value is incorrect - " << datapath_inst.mdr_reg << std::endl;
        std::cout << "MEM_LD2 Test Failed" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "----------------------------------------------" << std::endl;
	
    // Write-back stage for ld: write MDR value to destination register ($r0) and increment PC
    enable_mdr = false;
    s_regfile_rw = false;
    s_regfile_din = true;   // select MDR as data input to reg file
    reg_write = true;
    alu_op = sc_bv<ALU_OP_W>(10);  // PC increment
    enable_pc = true;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Write Back Cycle - WB_LD",1);
	
    // Register $0 should now contain 0x0003
    if (datapath_inst.register_file[0] == 0x0003) {
        std::cout << "The register_file[0] is the expected value of " << datapath_inst.register_file[0] << std::endl;
    } 
	else {
        std::cout << "The register_file[0] is an unexpected value of " << datapath_inst.register_file[0] << std::endl;
        std::cout << "WB_LD Test Failed" << std::endl;
        return EXIT_FAILURE;
    }
	
    // PC should now be 2
    if (datapath_inst.pc_reg == 2) {
        std::cout << "The PC is the expected value of " << datapath_inst.pc_reg << std::endl;
    } 
	else {
        std::cout << "The PC is an unexpected value of " << datapath_inst.pc_reg << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "WB_LD Test PASSED" << std::endl;

    // ** Instruction 0x8112 (ld $r1, $r3, 1) **
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Testing 0x8112 - ld $r1 $r3 1" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
	
    // Fetch next instruction (PC = 2)
    enable_pc = false;
    reg_write = false;
    alu_op = sc_bv<ALU_OP_W>(0);
    s_addr = false;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH",1);
	
    // Expect PC Address = 2
    if(out_addr_out.read() != 2) {
        std::cout << "PC Register expected 2, got " << datapath_inst.out_addr_out << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Got expected PC Address of " << datapath_inst.out_addr_out << "... IFETCH test passed" << std::endl;
    }

    std::cout << "----------------------------------------------" << std::endl;

    // Provide instruction 0x8112 on data_in
    enable_instruction = true;
    data_in = sc_bv<DATA_W>(0x8112);
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH2",1);

    // Opcode should be 8 (ld)
    if(out_opcode.read() != 8) {
        std::cout << "Opcode for ld expected 8, got " << datapath_inst.out_opcode << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Got expected opcode of " << datapath_inst.out_opcode << "... IFETCH2 test passed" << std::endl;
    }

    std::cout << "----------------------------------------------" << std::endl;
	
    enable_instruction = false;
    enable_a = true;
    enable_b = true;
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Decode Cycle",1);
	
    // Expect ALU regs: r1 was 0, r3 was 0? (We assume base address still 32 loaded via r2, simulation expects 0 and 32 again)
    if (datapath_inst.alu_a_reg == 0 && datapath_inst.alu_b_reg == 32) {
        std::cout << "Got expected values of " << datapath_inst.alu_a_reg << " and " << datapath_inst.alu_b_reg << std::endl;
        std::cout << "Instruction Decode PASSED" << std::endl;
    } 
	else {
        std::cout << "Got unexpected values of " << datapath_inst.alu_a_reg << " and " << datapath_inst.alu_b_reg << std::endl;
        std::cout << "Instruction Decode Failed" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "----------------------------------------------" << std::endl;
    enable_a = false;
    enable_b = false;
    alu_op = sc_bv<ALU_OP_W>(8);
    enable_alu_result = true;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Execution Cycle",1);
	
    // ALU result = 0x0021 (32 + 1) expected
    if (datapath_inst.alu_result_reg == 0x0021) {
        std::cout << "Got expected alu_result reg of " << datapath_inst.alu_result_reg << std::endl;
        std::cout << "... Execution Stage Test Passed!" << std::endl;
    } 
	else {
        std::cout << "The Alu Result Register value is " << datapath_inst.alu_result_reg << "... Execution Cycle Test Failed" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "----------------------------------------------" << std::endl;

    enable_alu_result = false;
    s_addr = true;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Load/Store Cycle - MEM_LD",1);
	
    // Expect addr_out = 33
    if(out_addr_out.read() == 33) {
        std::cout << "Got expected addr_out = " << datapath_inst.out_addr_out << std::endl;
        std::cout << "... MEM_LD Stage Test Passed!" << std::endl;
    } 
	else {
        std::cout << "The addr_out value is " << datapath_inst.out_addr_out << "... MEM_LD Test Failed" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "----------------------------------------------" << std::endl;

    s_addr = false;
    enable_mdr = true;
    data_in = 0x0004;  // data from memory[33]
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Load/Store Cycle - MEM_LD2",1);
	
    if (datapath_inst.mdr_reg == 0x0004) {
        std::cout << "The mdr_reg contains the expected value of " << datapath_inst.mdr_reg << std::endl;
        std::cout << "MEM_LD2 Test Passed" << std::endl;
    } 
	else {
        std::cout << "The mdr_reg value is incorrect - " << datapath_inst.mdr_reg << std::endl;
        std::cout << "MEM_LD2 Test Failed" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "----------------------------------------------" << std::endl;
    enable_mdr = false;
    s_regfile_rw = false;
    s_regfile_din = true;
    reg_write = true;
    alu_op = sc_bv<ALU_OP_W>(10);
    enable_pc = true;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Write Back Cycle - WB_LD",1);
	
    // Register $1 should contain 0x0004
    if (datapath_inst.register_file[1] == 0x0004) {
        std::cout << "The register_file[1] is the expected value of " << datapath_inst.register_file[1] << std::endl;
    } 
	else {
        std::cout << "The register_file[1] is an unexpected value of " << datapath_inst.register_file[1] << std::endl;
        std::cout << "WB_LD Test Failed" << std::endl;
        return EXIT_FAILURE;
    }

    // ** Instruction 0x0001 (add $r0, $r0, $r1) **
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Testing 0x0001 - add $r0 $r0 $r1" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    // Fetch next instruction (PC = 3)
    enable_pc = false;
    reg_write = false;
    alu_op = sc_bv<ALU_OP_W>(0);
    s_addr = false;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH",1);
	
    // Expect PC Address = 3
    if(out_addr_out.read() != 3) {
        std::cout << "PC Register expected 3, got " << datapath_inst.out_addr_out << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Got expected PC Address of " << datapath_inst.out_addr_out << "... IFETCH test passed" << std::endl;
    }

    std::cout << "----------------------------------------------" << std::endl;

    // Provide instruction 0x0001 on data_in
    enable_instruction = true;
    data_in = sc_bv<DATA_W>(0x0001);
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH2",1);
	
    // Opcode should be 0 (add)
    if(out_opcode.read() != 0) {
        std::cout << "Opcode for add expected 0, got " << datapath_inst.out_opcode << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Got expected opcode of " << datapath_inst.out_opcode << "... IFETCH2 test passed" << std::endl;
    }

    std::cout << "----------------------------------------------" << std::endl;

    enable_instruction = false;
    enable_a = true;
    enable_b = true;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Decode Cycle",1);
	
    // Expecting ALU regs to have r0 and r1 values: r0=3, r1=4
    if (datapath_inst.alu_a_reg == 3 && datapath_inst.alu_b_reg == 4) {
        std::cout << "Got expected values of " << datapath_inst.alu_a_reg << " and " << datapath_inst.alu_b_reg << std::endl;
        std::cout << "Instruction Decode PASSED" << std::endl;
    } 
	else {
        std::cout << "Got unexpected values of " << datapath_inst.alu_a_reg << " and " << datapath_inst.alu_b_reg << std::endl;
        std::cout << "Instruction Decode Failed" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "----------------------------------------------" << std::endl;

    enable_a = false;
    enable_b = false;
    alu_op = sc_bv<ALU_OP_W>(0);
    enable_alu_result = true;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Execute Cycle",1);

    // ALU result = 0x0007 (3 + 4)
    if (datapath_inst.alu_result_reg == 0x0007) {
        std::cout << "Got expected alu_result reg of " << datapath_inst.alu_result_reg << std::endl;
    } 
	else {
        std::cout << "The Alu Result Register value is " << datapath_inst.alu_result_reg << "... Execution Cycle Test Failed" << std::endl;
        return EXIT_FAILURE;
    }
	
    std::cout << "... Execution Stage Test Passed!" << std::endl;
	
    std::cout << "----------------------------------------------" << std::endl;

    // PC increment and write-back in WB_ALU (for add)
    std::cout << "Instruction cycle start time - " << current_time << std::endl;
    s_regfile_rw = false;
    s_regfile_din = false;
    reg_write = true;
    alu_op = sc_bv<ALU_OP_W>(10);
    enable_pc = true;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Write Back - WB_ALU",1);

	
    // PC after increment should be 4
    if (datapath_inst.alu_result_reg == 4) {
        std::cout << "Got expected alu_result reg of " << datapath_inst.alu_result_reg << std::endl;
    } else {
        std::cout << "The Alu Result Register value is " << datapath_inst.alu_result_reg << "... Execution Cycle Test Failed" << std::endl;
        return EXIT_FAILURE;
    }
	
    // Register $0 (dest) should contain 7
    if (datapath_inst.register_file[0] == 0x0007) {
        std::cout << "Got expected register_file[0] of " << datapath_inst.register_file[0] << std::endl;
    } 
	else {
        std::cout << "Got unexpected register_file[0] of " << datapath_inst.register_file[0] << std::endl;
        std::cout << "Write Back Test Failed" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "... Write Back Test Passed!" << std::endl;

    // ** Instruction 0x9202 (st $r0, 2($r2)) **
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Testing 0x9202 - st $r0 2($r2)" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    // Fetch next instruction (PC = 4)
    enable_pc = false;
    reg_write = false;
    alu_op = sc_bv<ALU_OP_W>(0);
    s_addr = false;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH",1);

	
    // Expect PC Address = 4
    if(out_addr_out.read() != 4) {
        std::cout << "PC Register expected 4, got " << datapath_inst.out_addr_out << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Got expected PC Address of " << datapath_inst.out_addr_out << "... IFETCH test passed" << std::endl;
    }

    std::cout << "----------------------------------------------" << std::endl;

    enable_instruction = true;
    data_in = sc_bv<DATA_W>(0x9202);
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH2",1);

    // Opcode should be 9 (st)
    if(out_opcode.read() != 9) {
        std::cout << "Opcode for st expected 9, got " << datapath_inst.out_opcode << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Got expected opcode of " << datapath_inst.out_opcode << "... IFETCH2 test passed" << std::endl;
    }

    std::cout << "----------------------------------------------" << std::endl;

    enable_instruction = false;
    enable_a = true;
    enable_b = true;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Decode Cycle",1);
	
    // Expect ALU A = 7 (r0), ALU B = 32 (r2)
    if (datapath_inst.alu_a_reg == 7 && datapath_inst.alu_b_reg == 32) {
        std::cout << "Got the expected values of alu_a = " << datapath_inst.alu_a_reg;
        std::cout << " and alu_b = " << datapath_inst.alu_b_reg << std::endl;
    } 
	else {
        std::cout << "Got unexpected values of alu_a = " << datapath_inst.alu_a_reg;
        std::cout << " or alu_b = " << datapath_inst.alu_b_reg << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "----------------------------------------------" << std::endl;

    enable_a = false;
    enable_b = false;
    alu_op = sc_bv<ALU_OP_W>(9);
    enable_alu_result = true;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Execute Cycle",1);
	
    // ALU result = 0x0022 (32 + 2)
    if (datapath_inst.alu_result_reg == 34) {
        std::cout << "Got expected alu_result reg of " << datapath_inst.alu_result_reg << std::endl;
    } 
	else {
        std::cout << "The Alu Result Register value is " << datapath_inst.alu_result_reg << "... Execution Cycle Test Failed" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "... Execution Stage Test Passed!" << std::endl;

    std::cout << "----------------------------------------------" << std::endl;

    // Memory stage for store: output address and data for memory write, and increment PC
    s_addr = true;      // select ALU result for addr_out
    alu_op = sc_bv<ALU_OP_W>(10);  // PC increment operation
    enable_pc = true;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Load/Store Cycle - MEM_ST",1);
	
    // Expect data_out = 7 (value of $r0)
    if(out_data_out.read() == sc_bv<DATA_W>(7)) {
        std::cout << "Expected value of data_out = " << datapath_inst.out_data_out << std::endl;
    } 
	else {
        std::cout << "Unexpected value of data_out = " << datapath_inst.out_data_out << std::endl;
        return EXIT_FAILURE;
    }    


    s_addr = false;      // select ALU result for addr_out
    alu_op = sc_bv<ALU_OP_W>(0);  // PC increment operation
    enable_pc = false;
    run_instruction_step(datapath_inst,current_time,cycle_time,"Load/Store Cycle - MEM_ST2",1);

    // Writing to memory map, no test
    std::cout << "Writing to memory map. No Test" << std::endl;


    // ** Instruction 0xF000 (quit) **
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Testing 0xF000 - quit" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    // Fetch next instruction (PC = 5)
    enable_pc = false;
    reg_write = false;
    alu_op = sc_bv<ALU_OP_W>(0);
    s_addr = false;
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH",1);

    // Expect PC Address = 5
    if(out_addr_out.read() != 5) {
        std::cout << "PC Register expected 5, got " << datapath_inst.out_addr_out << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Got expected PC Address of " << datapath_inst.out_addr_out << "... IFETCH test passed" << std::endl;
    }

    std::cout << "----------------------------------------------" << std::endl;

    enable_instruction = true;
    data_in = sc_bv<DATA_W>(0xF000);
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Instruction Fetch Cycle IFETCH2",1);

    // Opcode should be 0xF (quit)
    if(out_opcode.read() != 0xF) {
        std::cout << "Opcode for quit expected 0xF, got " << datapath_inst.out_opcode << std::endl;
        return EXIT_FAILURE;
    } 
	else {
        std::cout << "Got expected opcode of " << datapath_inst.out_opcode << "... IFETCH2 test passed" << std::endl;
    }

    // End of program - perform final reset test
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Running a reset test" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    reset.write(false);
	
    run_instruction_step(datapath_inst,current_time,cycle_time,"Run reset test", 1);

    if (datapath_inst.test_reset_datapath()) {
      std::cout << "Reset test PASSED" << std::endl;
    } 
    else {
      std::cout << "Reset test FAILED" << std::endl;
      return EXIT_FAILURE;
    }

    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Datapath Test completed successfully" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    return EXIT_SUCCESS;
}
