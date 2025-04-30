/*********************************************
 * File name: sc_memory_map.cpp
 * Author: Matthew Morrison
 * Date: 4/19/2025
 *
 * This file contains the sc_main function for
 * testing the albacore memory_map (refactored with step() method)
 *********************************************/

#include <systemc>
#include "sizes.h"
#include "memory_map.h"

void run_instruction_step( memory_map& memory_map_inst, sc_time& current_time, const sc_time& cycle_time, const std::string& detail, int num_cycles ){

  std::cout << detail << endl;
  std::cout << "Instruction cycle start time - " << current_time << std::endl;
  
  for(int i = 0; i < num_cycles; ++i) {
    sc_start(2*cycle_time);
    current_time += 2*cycle_time;
  }

}

int sc_main(int argc, char* argv[]) {

  // Create memory_map instance (plain C++ class)
  memory_map memory_map_inst("memory_map_inst");
  
  // Time management
  sc_time current_time = SC_ZERO_TIME;
  sc_time cycle_time(CYCLE_LENGTH, SC_NS);


  sc_clock clk("clk", CYCLE_LENGTH);
  sc_buffer<bool> reset;

  sc_buffer< sc_bv<ADDR_W> > mem_addr;
  sc_buffer< sc_bv<DATA_W> > mem_din;
  sc_buffer<bool> mem_we;
  sc_buffer< sc_bv<ADDR_W> > addr;            // Port 2
  sc_buffer< sc_bv<DATA_W> > din;             // Port 3
  sc_buffer<bool> we;                         // Port 4
  sc_buffer< sc_bv<4> > io_in;                // Port 5
  
  sc_buffer< sc_bv<DATA_W> > dout;           // Port 6
  sc_buffer< sc_bv<4> > io_out;              // Port 7


  memory_map_inst.clk(clk);
  memory_map_inst.reset(reset);


  memory_map_inst.mem_addr( mem_addr );
  memory_map_inst.mem_din( mem_din );
  memory_map_inst.mem_we( mem_we );
  memory_map_inst.addr( addr );            // Port 2
  memory_map_inst.din( din );             // Port 3
  memory_map_inst.we( we );                         // Port 4
  memory_map_inst.io_in( io_in );                // Port 5
  
  memory_map_inst.dout( dout );           // Port 6
  memory_map_inst.io_out( io_out );              // Port 7


  
  // Initialize and perform reset sequence
  std::cout << "Setting start to 0 and mem_mem_we to 0 at " << current_time << std::endl;
  std::cout << "-------------------------" << std::endl;
  
  mem_we = false;
  
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Start up with 1 cycles",1);  // 1 cycle with reset high
  
  std::cout << "-------------------------" << std::endl;
  std::cout << "Running a reset test" << std::endl;
  std::cout << "The reset block will take 2 cycles" << std::endl;
  std::cout << "-------------------------" << std::endl;

  reset.write(true);
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Run Reset Test",1);  // 1 cycle with reset high  

  if(!memory_map_inst.test_reset_memory_map()){
    std::cout << "Memory Map Reset Failed" << std::endl;
    exit(EXIT_FAILURE);
  }
  else{
    std::cout << "Memory Map Reset Passed" << std::endl;
  }
  
  
  // Program loading: write 8 values into memory (addresses 0-5, 32-33)
  std::cout << "-------------------------" << std::endl;
  std::cout << "Writing to the Memory RAM for 8 cycles" << std::endl;
  std::cout << "-------------------------" << std::endl;
  reset.write(true);
  mem_we = true;


  // Write 0x7220 at Memory[0]
  
  mem_addr = 0;
  mem_din = 0x7220;
  
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Write 0x7220 at Memory[0]",1);
  
  if (memory_map_inst.Memory[0] != 0x7220) {
    std::cout << "Unexpected value of Memory[0] = " << memory_map_inst.Memory[0] << std::endl;
    return EXIT_FAILURE;
  } 
  else {
    std::cout << "Expected value of Memory[0] = " << memory_map_inst.Memory[0] << std::endl;
    std::cout << "Memory Write Test Passed" << std::endl;
  }
  
  // Write 0x8002 at Memory[1]
  mem_addr = 1;
  mem_din = 0x8002;
  
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Write 0x8002 at Memory[1]",1);
  
  if (memory_map_inst.Memory[1] != 0x8002) {
    std::cout << "Unexpected value of Memory[1] = " << memory_map_inst.Memory[1] << std::endl;
    return EXIT_FAILURE;
  } 
  else {
    std::cout << "Expected value of Memory[1] = " << memory_map_inst.Memory[1] << std::endl;
    std::cout << "Memory Write Test Passed" << std::endl;
  }
  
  // Write 0x8112 at Memory[2]
  mem_addr = 2;
  mem_din = 0x8112;
  
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Write 0x8112 at Memory[2]",1);
  
  if (memory_map_inst.Memory[2] != 0x8112) {
    std::cout << "Unexpected value of Memory[2] = " << memory_map_inst.Memory[2] << std::endl;
    return EXIT_FAILURE;
  } 
  else {
    std::cout << "Expected value of Memory[2] = " << memory_map_inst.Memory[2] << std::endl;
    std::cout << "Memory Write Test Passed" << std::endl;
  }
  
  // Write 0x0001 at Memory[3]
  mem_addr = 3;
  mem_din = 0x0001;
  
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Write 0x0001 at Memory[3]",1);
  
  if (memory_map_inst.Memory[3] != 0x0001) {
    std::cout << "Unexpected value of Memory[3] = " << memory_map_inst.Memory[3] << std::endl;
    return EXIT_FAILURE;
  } 
  else {
    std::cout << "Expected value of Memory[3] = " << memory_map_inst.Memory[3] << std::endl;
    std::cout << "Memory Write Test Passed" << std::endl;
  }
  
  // Write 0x9202 at Memory[4]
  mem_addr = 4;
  mem_din = 0x9202;
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Write 0x9202 at Memory[4]",1);
  
  if (memory_map_inst.Memory[4] != 0x9202) {
    std::cout << "Unexpected value of Memory[4] = " << memory_map_inst.Memory[4] << std::endl;
    return EXIT_FAILURE;
  } 
  else {
    std::cout << "Expected value of Memory[4] = " << memory_map_inst.Memory[4] << std::endl;
    std::cout << "Memory Write Test Passed" << std::endl;
  }
  
  // Write 0xF000 at Memory[5]
  mem_addr = 5;
  mem_din = 0xF000;
  
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Write 0xF000 at Memory[5]",1);
  
  if (memory_map_inst.Memory[5] != 0xF000) {
    std::cout << "Unexpected value of Memory[5] = " << memory_map_inst.Memory[5] << std::endl;
    return EXIT_FAILURE;
  } 
  else {
    std::cout << "Expected value of Memory[5] = " << memory_map_inst.Memory[5] << std::endl;
    std::cout << "Memory Write Test Passed" << std::endl;
  }
  
  // Write 0x0003 at Memory[32]
  mem_addr = 32;
  mem_din = 0x0003;
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Write 0x0003 at Memory[32]",1);
  
  if (memory_map_inst.Memory[32] != 0x0003) {
    std::cout << "Unexpected value of Memory[32] = " << memory_map_inst.Memory[32] << std::endl;
  } 
  else {
    std::cout << "Expected value of Memory[32] = " << memory_map_inst.Memory[32] << std::endl;
    std::cout << "Memory Write Test Passed" << std::endl;
  }
  
  // Write 0x0004 at Memory[33]
  mem_addr = 33;
  mem_din = 0x0004;
  
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Write 0x0004 at Memory[33]",1);
  
  if (memory_map_inst.Memory[33] != 0x0004) {
    std::cout << "Unexpected value of Memory[33] = " << memory_map_inst.Memory[33] << std::endl;
    return EXIT_FAILURE;
  } 
  else {
    std::cout << "Expected value of Memory[33] = " << memory_map_inst.Memory[33] << std::endl;
    std::cout << "Memory Write Test Passed" << std::endl;
  }
  
  // Begin processor execution simulation (no actual CPU, we simulate cycles only)
  mem_we = false;
  
  std::cout << "-------------------------" << std::endl;
  
  std::cout << "Clearing reset and starting datapath tests" << std::endl;
  std::cout << "This test includes the six instructions from PLPD pg. 296" << std::endl;
  std::cout << "A compiling for the following C code:" << std::endl;
  std::cout << "int main(){" << std::endl;
  std::cout << "    A[0] = 2;" << std::endl;
  std::cout << "    A[1] = 3;" << std::endl;
  std::cout << "    A[2] = A[1] + A[0];" << std::endl;
  std::cout << "    return 0;" << std::endl;
  std::cout << "}" << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "-------------------------" << std::endl;
  
  
  // Start signal not present in memory_map, just run one cycle to represent CPU start
  
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Testing Start Signal",1);
  
  std::cout << "Start Test Passed" << std::endl;
  
  // Instruction cycles simulation (timing only, no input changes, just ensuring no unexpected behavior)
  std::cout << "-------------------------" << std::endl;
  
  // Run for 5 cycles to simulate this instruction execution
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Testing 0x7220 - ldi $2 32",5);
  
  std::cout << "-------------------------" << std::endl;
  
  // Run for 7 cycles
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Testing 0x8002 - ld $r0 $r2 0",7);
  
  std::cout << "-------------------------" << std::endl;
  
  
  // Run for 7 cycles
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Testing 0x8112 - ld $r1 $r3 1",7);
  
  std::cout << "-------------------------" << std::endl;
  
  // Run for 5 cycles
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Testing 0x0001 - add $r0 $r0 $r1",5);
  
  std::cout << "-------------------------" << std::endl;
  
  std::cout << "// Four cycles before write" << std::endl;
  // Set up a store operation to Memory[34]: write value 7 to address 34 via proc interface
  mem_we = false;
  mem_addr = 34;
  mem_din = 100;     // (dummy value not used in mem write)
  we = true;
  addr = 34;
  din = 7;
  
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Testing 0x9202 - st $r0 2($r2)",6);  // run 6 cycles (store completes)
  
  if (memory_map_inst.Memory[34] != 7) {
    std::cout << "Unexpected value of Memory[34] = " << memory_map_inst.Memory[34] << std::endl;
  } 
  else {
    std::cout << "Expected value of Memory[34] = " << memory_map_inst.Memory[34] << std::endl;
    std::cout << "Memory Write Test Passed" << std::endl;
  }
  
  std::cout << "-------------------------" << std::endl;
  
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Testing 0xF000 - quit",2);
  mem_we = false;  // ensure mem loader off
  
  // Final reset test
  std::cout << "-------------------------" << std::endl;
  reset.write(false);
  run_instruction_step(memory_map_inst,current_time,cycle_time,"Run Reset Test",1);  // 1 cycle with reset high    

  if(!memory_map_inst.test_reset_memory_map()){
    std::cout << "Memory Map Reset Failed" << std::endl;
    exit(EXIT_FAILURE);
  }
  else{
    std::cout << "Memory Map Reset Passed" << std::endl;
  }
  
  
  std::cout << "-------------------------" << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "Memory Map Test completed successfully" << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "-------------------------" << std::endl;

  sc_stop();

  return EXIT_SUCCESS;
}
