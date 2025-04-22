#include "memory_map.h"

memory_map::memory_map() {
  reset();
}

void memory_map::reset() {
  out_dout = 0;
  out_io_out = 0;

  for (int i = 0; i < RAM_SIZE; ++i) {
    Memory[i] = 0;
  }
}


void memory_map::compute() {
  // Compute address and input selection
  bool we = mem_we ? mem_we : proc_we;
  sc_bv<ADDR_W> addr = mem_we ? mem_addr : proc_addr;
  sc_bv<DATA_W> din = mem_we ? mem_din : proc_din;

  // Default output
  next_out_dout = 0;
  next_out_io_out = 0;

  // RAM or I/O write (done in-place immediately, no copy)
  if (we) {
    Memory[addr.to_uint()] = din;
  }

  // Read value from memory
  sc_bv<DATA_W> dout_ram = Memory[addr.to_uint()];

  // Mux the outputs
  if (addr[MSB_MEM] == 0) {
    next_out_dout = dout_ram;
  } else if (addr == sc_bv<ADDR_W>(0x8001)) {
    next_out_dout = (sc_bv<12>(0), io_in);  // top 12 bits 0, bottom 4 = io_in
  }

  // I/O output signal (triggered only by store to 0x8000)
  if (addr == sc_bv<ADDR_W>(0x8000) && we) {
    next_out_io_out = io_in;
  }

  out_dout = next_out_dout;

  out_io_out = next_out_io_out;

}

void memory_map::commit() {

}
