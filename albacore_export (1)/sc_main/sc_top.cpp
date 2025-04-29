/*********************************************
 * File name: sc_top.cpp
 * Author: Matthew Morrison
 * Date: 4/19/2025
 *
 * This file contains the sc_main function for
 * testing the integrated top module (using step() method).
 *********************************************/

#include <systemc>
#include <stratus_hls.h>
#include "../src/top.h"

void run_instruction_step(sc_time& current_time, const sc_time& cycle_time, const std::string& detail, int num_cycles) {
    std::cout << detail << std::endl;
    std::cout << "Instruction cycle start time - " << current_time << std::endl;
    for (int i = 0; i < num_cycles; ++i) {
        sc_start(cycle_time);
        current_time += cycle_time;
    }
}

void memory_map_test(const memory_map& memory_map_inst, const sc_bv<ADDR_W>& addr, const sc_bv<DATA_W>& data) {
    if (memory_map_inst.Memory[addr.to_uint()] != data) {
        std::cout << "Unexpected value of Memory[" << addr.to_uint() << "] = " << memory_map_inst.Memory[addr.to_uint()] << std::endl;
        sc_stop();
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Expected value of Memory[" << addr.to_uint() << "] = " << memory_map_inst.Memory[addr.to_uint()] << std::endl;
        std::cout << "Memory Write Test Passed" << std::endl;
    }
}

void register_test(const datapath& datapath_inst, const unsigned int register_addr, const unsigned int register_val){

  cout << "Testing datapath_inst.register_file[" << register_addr << "] ...";
  
    if (datapath_inst.register_file[register_addr] != register_val) {
        std::cout << "LDI failed: Got " << datapath_inst.register_file[register_addr] << std::endl;
        sc_stop();
        exit(EXIT_FAILURE);
    }
    else{
        std::cout << "LDI passed: Got " << datapath_inst.register_file[register_addr] << std::endl;
    }

}

int sc_main(int argc, char* argv[]) {
    top top_inst("top_inst");

    sc_time current_time = SC_ZERO_TIME;
    sc_time cycle_time(CYCLE_LENGTH, SC_NS);

    sc_clock clk("clk", CYCLE_LENGTH);
    sc_buffer<bool> reset;

    top_inst.clk(clk);
    top_inst.reset(reset);

    sc_buffer<bool> in_start;
    sc_buffer<sc_bv<ADDR_W>> in_mem_addr;
    sc_buffer<sc_bv<DATA_W>> in_mem_data;
    sc_buffer<bool> in_mem_we;
    sc_buffer<sc_bv<4>> in_io_in;
    sc_buffer<sc_bv<DATA_W>> out_mem_data_out;
    sc_buffer<sc_bv<4>> out_io_out;

    top_inst.start(in_start);
    top_inst.mem_addr(in_mem_addr);
    top_inst.mem_data(in_mem_data);
    top_inst.mem_we(in_mem_we);
    top_inst.io_in(in_io_in);
    top_inst.mem_data_out(out_mem_data_out);
    top_inst.io_out(out_io_out);

    std::cout << "-------------------------" << std::endl;
    reset.write(true);
    run_instruction_step(current_time, cycle_time, "Start up with 2 cycles (reset high)", 1);

    std::cout << "-------------------------" << std::endl;
    reset.write(false);
    run_instruction_step(current_time, cycle_time, "Reset active", 1);

    reset.write(true);
    in_start.write(false);
    in_mem_we.write(true);

    std::cout << "-------------------------" << std::endl;

    in_mem_addr.write(0); in_mem_data.write(0x7220);
    run_instruction_step(current_time, cycle_time, "Write 0x7220 to RAM[0]", 2);
    memory_map_test(top_inst.memory_map_inst, 0, 0x7220);

    std::cout << "-------------------------" << std::endl;

    in_mem_addr.write(1); in_mem_data.write(0x8002);
    run_instruction_step(current_time, cycle_time, "Write 0x8002 to RAM[1]", 2);
    memory_map_test(top_inst.memory_map_inst, 1, 0x8002);

    std::cout << "-------------------------" << std::endl;

    in_mem_addr.write(2); in_mem_data.write(0x8112);
    run_instruction_step(current_time, cycle_time, "Write 0x8112 to RAM[2]", 2);
    memory_map_test(top_inst.memory_map_inst, 2, 0x8112);

    std::cout << "-------------------------" << std::endl;

    in_mem_addr.write(3); in_mem_data.write(0x0001);
    run_instruction_step(current_time, cycle_time, "Write 0x0001 to RAM[3]", 2);
    memory_map_test(top_inst.memory_map_inst, 3, 0x0001);

    std::cout << "-------------------------" << std::endl;

    in_mem_addr.write(4); in_mem_data.write(0x9202);
    run_instruction_step(current_time, cycle_time, "Write 0x9202 to RAM[4]", 2);
    memory_map_test(top_inst.memory_map_inst, 4, 0x9202);

    std::cout << "-------------------------" << std::endl;

    in_mem_addr.write(5); in_mem_data.write(0xF000);
    run_instruction_step(current_time, cycle_time, "Write 0xF000 to RAM[5]", 2);
    memory_map_test(top_inst.memory_map_inst, 5, 0xF000);

    std::cout << "-------------------------" << std::endl;

    in_mem_addr.write(32); 
    in_mem_data.write(3);
    run_instruction_step(current_time, cycle_time, "Write 0x0003 to RAM[32]", 2);
    memory_map_test(top_inst.memory_map_inst, 32, 3);

    std::cout << "-------------------------" << std::endl;

    in_mem_addr.write(33); in_mem_data.write(4);
    run_instruction_step(current_time, cycle_time, "Write 0x0004 to RAM[33]", 2);
    memory_map_test(top_inst.memory_map_inst, 33, 4);

    std::cout << "-------------------------" << std::endl;

    in_mem_we.write(false);
    in_start.write(true);
    run_instruction_step(current_time, cycle_time, "Start program", 2);

    std::cout << "-------------------------" << std::endl;
    in_start.write(false);
    run_instruction_step(current_time, cycle_time, "Start program clear", 2);

    std::cout << "-------------------------" << std::endl;

    run_instruction_step(current_time, cycle_time, "0x7220 - ldi $2, 32", 10);
    register_test(top_inst.datapath_inst, 2, 32);

    std::cout << "-------------------------" << std::endl;

    run_instruction_step(current_time, cycle_time, "0x8002 - ld $0, $2, 0", 14);
    register_test(top_inst.datapath_inst, 0, 3);

    std::cout << "-------------------------" << std::endl;

    run_instruction_step(current_time, cycle_time, "0x8112 - ld $1, $2, 1", 14);
    register_test(top_inst.datapath_inst, 1, 4);

    std::cout << "-------------------------" << std::endl;

    run_instruction_step(current_time, cycle_time, "0x0001 - add $0, $0, $1", 10);
    register_test(top_inst.datapath_inst, 0, 7);

    std::cout << "-------------------------" << std::endl;

    run_instruction_step(current_time, cycle_time, "0x9202 - st $0, 2($2)", 12);

    memory_map_test(top_inst.memory_map_inst, 34, 7);

    std::cout << "-------------------------" << std::endl;

    run_instruction_step(current_time, cycle_time, "0xF000 - quit", 2);

    std::cout << "-------------------------" << std::endl;

    reset.write(false);
    run_instruction_step(current_time, cycle_time, "Final reset", 1);

    std::cout << "-------------------------" << std::endl;

    std::cout << "Top-Level Test Completed Successfully!" << std::endl;
    return EXIT_SUCCESS;
}
