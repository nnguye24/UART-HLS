#include "top.h"

void top::process() {
  // Initial reset


  while (true) {
    if (!reset.read()) {
      HLS_DEFINE_PROTOCOL("reset");

      datapath_inst.reset();
      controller_inst.reset();
      memory_map_inst.reset();
    } else {
      compute();
      commit();
    }

    {
      HLS_DEFINE_PROTOCOL("wait");
      wait(); // Next cycle
    }
  }
}

void top::compute() {
  // === INPUT PHASE ===
  bool in_start = start.read();
  bool in_mem_we = mem_we.read();
  sc_bv<ADDR_W> in_mem_addr = mem_addr.read();
  sc_bv<DATA_W> in_mem_data = mem_data.read();
  sc_bv<4> in_io_in = io_in.read();

  // === CONTROLLER INPUTS ===
  controller_inst.start = in_start;
  controller_inst.mem_we = in_mem_we;
  controller_inst.zero = datapath_inst.out_zero;
  controller_inst.neg = datapath_inst.out_neg;
  controller_inst.opcode = datapath_inst.out_opcode;

  memory_map_inst.proc_addr = datapath_inst.out_addr_out;
  memory_map_inst.proc_din = datapath_inst.out_data_out;

  // === MEMORY INPUTS ===
  memory_map_inst.mem_addr = in_mem_addr;
  memory_map_inst.mem_din = in_mem_data;
  memory_map_inst.mem_we = in_mem_we;
  memory_map_inst.proc_we = controller_inst.out_we_mem;
  memory_map_inst.io_in = in_io_in;

  controller_inst.compute();
  memory_map_inst.compute();



  // === DATAPATH INPUTS ===
  datapath_inst.start = in_start;
  datapath_inst.mem_we = in_mem_we;
  datapath_inst.alu_op = controller_inst.out_alu_op;
  datapath_inst.data_in = memory_map_inst.out_dout;
  datapath_inst.enable_instruction = controller_inst.out_enable_instr;  
  datapath_inst.enable_a = controller_inst.out_enable_a;
  datapath_inst.enable_b = controller_inst.out_enable_b;
  datapath_inst.enable_pc = controller_inst.out_enable_pc;
  datapath_inst.enable_alu_result = controller_inst.out_enable_alu_result;
  datapath_inst.enable_mdr = controller_inst.out_enable_mdr;
  datapath_inst.s_regfile_rw = controller_inst.out_s_regfile_rw;
  datapath_inst.s_regfile_din = controller_inst.out_s_regfile_din;
  datapath_inst.reg_write = controller_inst.out_reg_write;
  datapath_inst.s_addr = controller_inst.out_s_addr;

  // === COMPUTE PHASE ===
  datapath_inst.compute();


}

void top::commit() {
  datapath_inst.commit();
  controller_inst.commit();
  memory_map_inst.commit();

  // === OUTPUT PHASE ===
  mem_data_out.write(memory_map_inst.out_dout);
  io_out.write(memory_map_inst.out_io_out);
}
