#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "systemc.h"
#include "sizes.h"

class controller {
public:
  // Inputs
  bool rst;
  bool tx_buffer_full;
  bool rx_buffer_empty;
  bool parity_error;
  bool framing_error;
  bool overrun_error;
  bool rx_in;  // Serial input line for detecting start bit

  // FSM state
  sc_bv<4> state;
  sc_bv<4> next_state;
  
  // Internal signals and counters
  int bit_counter;    // Counts bits transmitted/received
  bool parity_value;  // Calculated parity value
  bool parity_enabled; // Configuration parameter
  bool tx_done;       // Transmission complete flag
  bool rx_done;       // Reception complete flag

  // Outputs
  bool out_load_tx;
  bool out_tx_start;
  bool out_tx_data;
  bool out_tx_parity;
  bool out_tx_stop;
  bool out_rx_start;
  bool out_rx_data;
  bool out_rx_parity;
  bool out_rx_stop;
  bool out_error_handle;

  // Constructor
  controller();
  
  // Reset method
  void reset();
  
  // Methods matching albacore pattern
  void compute();
  void commit();

private:
  // Latched outputs from compute()
  bool load_tx_next;
  bool tx_start_next;
  bool tx_data_next;
  bool tx_parity_next;
  bool tx_stop_next;
  bool rx_start_next;
  bool rx_data_next;
  bool rx_parity_next;
  bool rx_stop_next;
  bool error_handle_next;
};

#endif