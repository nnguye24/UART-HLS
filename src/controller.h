#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "systemc.h"
#include "sizes.h"

SC_MODULE(controller) {
public:
  // Inputs
  sc_in<bool> clk;   //port 0
  sc_in<bool> rst;   // port 1
  sc_in<bool> tx_buffer_full; // port 2
  sc_in<bool> rx_buffer_empty; // port 3
  sc_in<bool> rx_in;  // Serial input line //port 4
  // Error inputs
  sc_in<bool> parity_error; //port 5
  sc_in<bool> framing_error; //port 6
  sc_in<bool> overrun_error; //port 7
  
  // Configuration inputs from datapath
  sc_in<bool> parity_enabled;        // Parity enabled flag //port 8
  sc_in<bool> parity_even;           // Even parity (1) or odd parity (0) //port 9
  sc_uint<3> data_bits;       // Number of data bits (5-8) //port 10
  sc_uint<2> stop_bits;       // Number of stop bits (1, 1.5, 2) //port 11
  
  // Output control signals
  sc_out<bool> out_load_tx; //port 12
  sc_out<bool> out_load_tx2;          // Added second phase load signal port 13
  sc_out<bool> out_tx_start; //port 14
  sc_out<bool> out_tx_data; //port 15
  sc_out<bool> out_tx_parity; //port 16
  sc_out<bool>  out_tx_stop; //port 17
  sc_out<bool> out_rx_start; //port 18
  sc_out<bool> out_rx_data; //port 19
  sc_out<bool> out_rx_parity; //port 20
  sc_out<bool> out_rx_stop; //port 21
  sc_out<bool> out_rx_read; //port 22
  sc_out<bool> out_error_handle; //port 23
  
  // TX FSM state
  sc_bv<4> tx_state;
  sc_bv<4> tx_next_state;
  sc_uint tx_bit_counter;      // Counts bits transmitted
  sc_bv<bool> tx_parity_value;    // Calculated parity value
  sc_bv<bool> tx_done;            // Transmission complete flag
  sc_bv<bool> tx_data_register;   // Current bit being transmitted

  // RX FSM state
  sc_bv<4> rx_state;
  sc_bv<4> rx_next_state;
  sc_uint<3> rx_bit_counter;      // Counts bits received
  sc_in<bool> rx_parity_value;    // Calculated parity value
  sc_in<bool> rx_done;            // Reception complete flag
  sc_bv<8> rx_shift_reg;   // Shift register to store received bits
  sc_in<bool> rx_bit_value;       // Current received bit

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
  sc_uint<1> load_tx_next;
  sc_uint<1> load_tx2_next;      // Added second phase load signal
  sc_uint<1> tx_start_next;
  sc_uint<1> tx_data_next;
  sc_uint<1> tx_parity_next;
  sc_unit<1>  tx_stop_next;

  // Latched RX outputs from compute_rx()
  sc_unit<1> rx_start_next;
  sc_uint<1> rx_data_next;
  sc_uint<1> rx_parity_next;
  sc_uint<1> rx_stop_next;
  sc_uint<1> error_handle_next;
};

#endif
