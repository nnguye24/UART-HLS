/**************************************************************
 * File Name: datapath.h
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/22/2025
 *
 * Datapath definition for UART controller
 **************************************************************/

 #ifndef __DATAPATH_H__
 #define __DATAPATH_H__
 
 #include "systemc.h"
 #include "sizes.h"
 #include "stratus_hls.h"
 
 class datapath {
 public:
     // Inputs from controller
     bool clk;
     bool rst;
     
     // Control signals from controller
     bool load_tx;
     bool tx_start;
     bool tx_data;
     bool tx_parity;
     bool tx_stop;
     bool rx_start;
     bool rx_data;
     bool rx_parity;
     bool rx_stop;
     bool error_handle;
     
     // Status signals (outputs to controller)
     bool tx_buffer_full;
     bool rx_buffer_empty;
     bool parity_error;
     bool framing_error;
     bool overrun_error;
     
     // External interface
     bool rx_in;                       // Serial input
     bool tx_out;                      // Serial output
     sc_bv<DATA_W> data_in;            // Data input from memory map
     sc_bv<DATA_W> data_out;           // Data output to memory map
     sc_bv<ADDR_W> addr;               // Address from memory map
     
     // Internal registers
     sc_bv<DATA_W> tx_shift_register;      // Transmit shift register
     sc_bv<DATA_W> rx_shift_register;      // Receive shift register
     sc_bv<8> tx_buffer[TX_BUFFER_SIZE];   // Transmit buffer
     sc_bv<8> rx_buffer[RX_BUFFER_SIZE];   // Receive buffer
     
     // Buffer pointers and counters
     unsigned int tx_buf_head;    // Head pointer for TX buffer
     unsigned int tx_buf_tail;    // Tail pointer for TX buffer
     unsigned int rx_buf_head;    // Head pointer for RX buffer
     unsigned int rx_buf_tail;    // Tail pointer for RX buffer
     unsigned int tx_bit_count;   // Counter for TX bits
     unsigned int rx_bit_count;   // Counter for RX bits
     
     // Baud rate generation
     sc_uint<16> baud_divider;    // Baud rate divider value
     sc_uint<16> baud_counter;    // Counter for baud rate generation
     
     // Configuration registers
     bool parity_enabled;         // Parity enabled flag
     bool parity_even;            // Even parity (1) or odd parity (0)
     sc_uint<3> data_bits;        // Number of data bits (5-8)
     sc_uint<2> stop_bits;        // Number of stop bits (1, 1.5, 2)
     
     // Constructor
     datapath();
     
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
     
     // Status methods
     bool tx_buffer_check();
     bool rx_buffer_check();
     // Parity method
     bool calculate_parity(sc_bv<8> data);
     // Accessing Memory Method
     void load_tx_register();
     void store_rx_register();
     
 private:
     // Internal next-state values for TX (computed in compute_tx phase)
     bool next_tx_buffer_full;
     bool next_tx_out;
     sc_bv<DATA_W> next_tx_shift_register;
     
     // Internal next-state values for RX (computed in compute_rx phase)
     bool next_rx_buffer_empty;
     bool next_parity_error;
     bool next_framing_error;
     bool next_overrun_error;
     sc_bv<DATA_W> next_rx_shift_register;
     sc_bv<DATA_W> next_data_out;
 };
 
 #endif