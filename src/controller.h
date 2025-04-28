#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "systemc.h"
#include "sizes.h"

class controller {
public:
  // Inputs
  bool clk;
  bool rst;
  bool tx_buffer_full;
  bool rx_buffer_empty;
  bool rx_in;  // Serial input line
  
  // Error inputs
  bool parity_error;
  bool framing_error;
  bool overrun_error;
  
  // Configuration inputs from datapath
  bool parity_enabled;        // Parity enabled flag
  bool parity_even;           // Even parity (1) or odd parity (0)
  sc_uint<3> data_bits;       // Number of data bits (5-8)
  sc_uint<2> stop_bits;       // Number of stop bits (1, 1.5, 2)
  
  // Output control signals
  bool out_load_tx;
  bool out_tx_start;
  bool out_tx_data;
  bool out_tx_parity;
  bool out_tx_stop;
  bool out_rx_start;
  bool out_rx_data;
  bool out_rx_parity;
  bool out_rx_stop;
  bool out_rx_read;
  bool out_error_handle;
  
  // TX FSM state
  sc_bv<4> tx_state;
  sc_bv<4> tx_next_state;
  int tx_bit_counter;      // Counts bits transmitted
  bool tx_parity_value;    // Calculated parity value
  bool tx_done;            // Transmission complete flag
  bool tx_data_register;   // Current bit being transmitted

  // RX FSM state
  sc_bv<4> rx_state;
  sc_bv<4> rx_next_state;
  int rx_bit_counter;      // Counts bits received
  bool rx_parity_value;    // Calculated parity value
  bool rx_done;            // Reception complete flag
  sc_bv<8> rx_shift_reg;   // Shift register to store received bits
  bool rx_bit_value;       // Current received bit

  // Constructor
  controller();
  
  // Reset method
  void reset();
  
  // Main compute/commit methods
  void compute();
  void commit();
  
  // Separate compute/commit methods for TX and RX
  void compute_tx();
  void compute_rx();
  void commit_tx();
  void commit_rx();
  
  // Helper methods for error detection
  bool calculate_parity(sc_bv<8> data);

private:
  // Latched TX outputs from compute_tx()
  bool load_tx_next;
  bool tx_start_next;
  bool tx_data_next;
  bool tx_parity_next;
  bool tx_stop_next;

  // Latched RX outputs from compute_rx()
  bool rx_start_next;
  bool rx_data_next;
  bool rx_parity_next;
  bool rx_stop_next;
  bool error_handle_next;
};

#endif