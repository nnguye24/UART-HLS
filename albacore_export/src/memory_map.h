#ifndef __MEMORY_MAP_H__
#define __MEMORY_MAP_H__

#include "systemc.h"
#include "sizes.h"
#include "stratus_hls.h"

class memory_map {
 public:
  // Internal RAM
  sc_bv<DATA_W> Memory[RAM_SIZE];
  
  // Inputs
  sc_bv<ADDR_W> mem_addr;
  sc_bv<DATA_W> mem_din;
  bool mem_we;

  sc_bv<ADDR_W> proc_addr;
  sc_bv<DATA_W> proc_din;
  bool proc_we;

  sc_bv<4> io_in;

  // Outputs
  sc_bv<DATA_W> out_dout;
  sc_bv<4> out_io_out;

  // Constructor
  memory_map();

  // Reset memory
  void reset();

   // Phase split
  void compute();
  void commit();

 private:
  // Temporary values computed in compute()
  sc_bv<DATA_W> next_out_dout;
  sc_bv<4> next_out_io_out;

};

#endif
