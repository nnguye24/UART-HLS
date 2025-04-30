#include "top.h"

void top::process() {
  // Initial reset
  while (true) {
    if (!rst.read()) {
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
  // Read inputs from signals
  bool in_rst = rst.read();
  bool in_chip_select = chip_select.read();
  bool in_read_write = read_write.read();
  bool in_write_enable = write_enable.read();
  bool in_rx_in = rx_in.read();
  sc_uint<DATA_W> in_data_in = data_in.read();
  sc_uint<ADDR_W> in_addr = addr.read();
  
  // === MEMORY MAP INPUTS ===
  memory_map_inst.rst = in_rst;
  memory_map_inst.data_in = in_data_in;
  memory_map_inst.addr = in_addr;
  memory_map_inst.chip_select = in_chip_select;
  memory_map_inst.read_write = in_read_write;
  memory_map_inst.write_enable = in_write_enable;
  
  // Connect status signals from datapath to memory map
  memory_map_inst.tx_buffer_full = datapath_inst.tx_buffer_full;
  memory_map_inst.rx_buffer_empty = datapath_inst.rx_buffer_empty;
  memory_map_inst.error_indicator = datapath_inst.parity_error || 
                                   datapath_inst.framing_error || 
                                   datapath_inst.overrun_error;
                                   
  // Connect datapath signals to memory map
  memory_map_inst.dp_data_in = datapath_inst.dp_data_in;
  memory_map_inst.dp_addr = datapath_inst.dp_addr;
  memory_map_inst.dp_write_enable = datapath_inst.dp_write_enable;
  
  // Compute memory map
  memory_map_inst.compute();
  
  // === CONTROLLER INPUTS ===
  controller_inst.clk = clk.read();
  controller_inst.rst = in_rst;
  controller_inst.tx_buffer_full = datapath_inst.tx_buffer_full;
  controller_inst.rx_buffer_empty = datapath_inst.rx_buffer_empty;
  controller_inst.rx_in = in_rx_in;
  controller_inst.parity_error = datapath_inst.parity_error;
  controller_inst.framing_error = datapath_inst.framing_error;
  controller_inst.overrun_error = datapath_inst.overrun_error;
  
  // Get configuration from datapath
  controller_inst.parity_enabled = datapath_inst.ctrl_parity_enabled;
  controller_inst.parity_even = datapath_inst.ctrl_parity_even;
  controller_inst.data_bits = datapath_inst.ctrl_data_bits;
  controller_inst.stop_bits = datapath_inst.ctrl_stop_bits;
  
  // Compute controller FSM
  controller_inst.compute();
  
  // === DATAPATH INPUTS ===
  datapath_inst.clk = clk.read();
  datapath_inst.rst = in_rst;
  
  // Control signals from controller
  datapath_inst.load_tx = controller_inst.out_load_tx;
  datapath_inst.load_tx2 = controller_inst.out_load_tx2; // Added missing load_tx2 signal
  datapath_inst.tx_start = controller_inst.out_tx_start;
  datapath_inst.tx_data = controller_inst.out_tx_data;
  datapath_inst.tx_parity = controller_inst.out_tx_parity;
  datapath_inst.tx_stop = controller_inst.out_tx_stop;
  datapath_inst.rx_start = controller_inst.out_rx_start;
  datapath_inst.rx_data = controller_inst.out_rx_data;
  datapath_inst.rx_parity = controller_inst.out_rx_parity;
  datapath_inst.rx_stop = controller_inst.out_rx_stop;
  datapath_inst.error_handle = controller_inst.out_error_handle;
  datapath_inst.rx_read = controller_inst.out_rx_read;
  
  // Data from memory map
  datapath_inst.data_in = memory_map_inst.data_out;
  // RX input signal
  datapath_inst.rx_in = in_rx_in;
  
  // Compute datapath
  datapath_inst.compute();
}

void top::commit() {
  // Commit state updates for all modules
  datapath_inst.commit();
  controller_inst.commit();
  memory_map_inst.commit();
  
  // Synchronize configuration between datapath and controller
  datapath_inst.sync_controller_config();
  
  // === OUTPUT PINS ===
  data_out.write(memory_map_inst.data_out);
  tx_out.write(datapath_inst.tx_out);
  tx_buffer_full.write(datapath_inst.tx_buffer_full);
  rx_buffer_empty.write(datapath_inst.rx_buffer_empty);
  error_indicator.write(datapath_inst.parity_error || 
                        datapath_inst.framing_error || 
                        datapath_inst.overrun_error);
}