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
  bool in_tx_start = tx_start.read();
  sc_bv<DATA_W> in_tx_data = tx_data.read();
  bool in_rx_in = rx_in.read();

  // === CONTROLLER INPUTS ===
  controller_inst.tx_start = in_tx_start;
  controller_inst.tx_done = tx_inst.out_tx_done;
  controller_inst.rx_done = rx_inst.out_rx_done;

  // === COMPUTE PHASE ===
  controller_inst.compute();

  // === TX INPUTS ===
  tx_inst.tx_start = in_tx_start;
  tx_inst.tx_data = in_tx_data;
  tx_inst.baud_tick = controller_inst.out_baud_tick;
  tx_inst.compute();

  // === RX INPUTS ===
  rx_inst.rx_in = in_rx_in;
  rx_inst.baud_tick = controller_inst.out_baud_tick;
  rx_inst.compute();
}

void uart_top::commit() {
  tx_inst.commit();
  rx_inst.commit();
  controller_inst.commit();

  // === OUTPUT PHASE ===
  tx_out.write(tx_inst.out_tx_out);
  tx_busy.write(tx_inst.out_tx_busy);
  rx_data.write(rx_inst.out_rx_data);
  rx_ready.write(rx_inst.out_rx_ready);
}