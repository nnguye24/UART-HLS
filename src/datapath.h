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
 #include "stratus_hls.h"
 #include "sizes.h"
 
 SC_MODULE(datapath) {
     // Clock and reset
     sc_in<bool> clk;                  // Port 0
     sc_in<bool> rst;                  // Port 1
     
     // Control signals from controller
     sc_in<bool> load_tx;              // Port 2
     sc_in<bool> load_tx2;             // Port 3
     sc_in<bool> tx_start;             // Port 4
     sc_in<bool> tx_data;              // Port 5
     sc_in<bool> tx_parity;            // Port 6
     sc_in<bool> tx_stop;              // Port 7
     sc_in<bool> rx_start;             // Port 8
     sc_in<bool> rx_data;              // Port 9
     sc_in<bool> rx_parity;            // Port 10
     sc_in<bool> rx_stop;              // Port 11
     sc_in<bool> error_handle;         // Port 12
     sc_in<bool> rx_read;              // Port 13
     
     // Status signals (outputs to controller)
     sc_out<bool> tx_buffer_full;      // Port 14
     sc_out<bool> rx_buffer_empty;     // Port 15
     sc_out<bool> parity_error;        // Port 16
     sc_out<bool> framing_error;       // Port 17
     sc_out<bool> overrun_error;       // Port 18
     
     // Configuration outputs to controller
     sc_out<bool> ctrl_parity_enabled; // Port 19
     sc_out<bool> ctrl_parity_even;    // Port 20
     sc_out<sc_uint<3>> ctrl_data_bits;// Port 21
     sc_out<sc_uint<2>> ctrl_stop_bits;// Port 22
     sc_out<sc_uint<16>> baud_divisor;
     
     // External interface
     sc_in<bool> rx_in;                // Port 23 - Serial input
     sc_out<bool> tx_out;              // Port 24 - Serial output
     sc_in<sc_bv<DATA_W>> data_in;     // Port 25 - Data input from memory map
     sc_out<sc_bv<DATA_W>> data_out;   // Port 26 - Data output to memory map
     sc_out<sc_bv<ADDR_W>> addr;       // Port 27 - Address to memory map
     
     // Interface to memory map for direct writes
     sc_out<sc_bv<DATA_W>> dp_data_in;      // Port 28 - Data to write to memory
     sc_out<sc_bv<ADDR_W>> dp_addr;         // Port 29 - Address to write to
     sc_out<bool> dp_write_enable;          // Port 30 - Write enable signal
     
     // Memory management inputs
     sc_in<bool> start;                // Port 31 - Start signal
     sc_in<bool> mem_we;               // Port 32 - Memory write enable
     
     // Main process method
     void process();
     
     // Core methods
     void reset();
     void read_inputs();
     void write_outputs();
     void compute();
     void commit();
     
     // Processing methods
     void update_configuration();
     void compute_tx();
     void compute_rx();
     void sync_controller_config();
     
     // Helper methods
     bool calculate_parity(sc_bv<8> data);
     bool tx_buffer_check();
     
     // Internal registers
     sc_bv<DATA_W> tx_shift_register;  // Transmit shift register
     sc_bv<DATA_W> rx_shift_register;  // Receive shift register
     
     // Buffer pointers
     unsigned int tx_buf_head;    // Head pointer for TX buffer in memory
     unsigned int tx_buf_tail;    // Tail pointer for TX buffer in memory
     unsigned int rx_buf_head;    // Head pointer for RX buffer in memory
     unsigned int rx_buf_tail;    // Tail pointer for RX buffer in memory
     
     // Bit counters
     unsigned int tx_bit_count;   // Counter for TX bits
     unsigned int rx_bit_count;   // Counter for RX bits
     
     // Baud rate generation
     sc_uint<16> baud_divider;    // Baud rate divisor value
     sc_uint<16> baud_counter;    // Counter for baud rate generation
     
     // Configuration registers
     bool parity_enabled;         // Parity enabled flag
     bool parity_even;            // Even parity (1) or odd parity (0)
     sc_uint<3> data_bits;        // Number of data bits (5-8)
     sc_uint<2> stop_bits;        // Number of stop bits (1, 1.5, 2)
     
     // Internal state variables
     bool load_tx_phase;          // State variable for load_tx two-phase operation
     
     // Internal input values
     sc_bit in_start;
     sc_bit in_mem_we;
     sc_bit in_load_tx;
     sc_bit in_load_tx2;
     sc_bit in_tx_start;
     sc_bit in_tx_data;
     sc_bit in_tx_parity;
     sc_bit in_tx_stop;
     sc_bit in_rx_start;
     sc_bit in_rx_data;
     sc_bit in_rx_parity;
     sc_bit in_rx_stop;
     sc_bit in_error_handle;
     sc_bit in_rx_read;
     sc_bit in_rx_in;
     sc_bv<DATA_W> in_data_in;
     
     // Internal output values
     sc_bit out_tx_buffer_full;
     sc_bit out_rx_buffer_empty;
     sc_bit out_parity_error;
     sc_bit out_framing_error;
     sc_bit out_overrun_error;
     sc_bit out_ctrl_parity_enabled;
     sc_bit out_ctrl_parity_even;
     sc_uint<3> out_ctrl_data_bits;
     sc_uint<2> out_ctrl_stop_bits;
     sc_bit out_tx_out;
     sc_bv<DATA_W> out_data_out;
     sc_bv<ADDR_W> out_addr;
     sc_bv<DATA_W> out_dp_data_in;
     sc_bv<ADDR_W> out_dp_addr;
     sc_bit out_dp_write_enable;
     
     // Next-state values
     bool next_tx_buffer_full;
     bool next_tx_out;
     sc_bv<DATA_W> next_tx_shift_register;
     bool next_rx_buffer_empty;
     unsigned int next_rx_buf_head;
     unsigned int next_rx_buf_tail;
     bool next_parity_error;
     bool next_framing_error;
     bool next_overrun_error;
     sc_bv<DATA_W> next_rx_shift_register;
     sc_bv<DATA_W> next_data_out;
     
     // Constructor
     SC_CTOR(datapath) {
         SC_THREAD(process);
         sensitive << clk.pos();
         async_reset_signal_is(rst, false);
     }
     
 #ifdef NC_SYSTEMC
 public:
     void ncsc_replace_names() {
         // Replace port names for simulation
         ncsc_replace_name(clk, "clk");                    // Port 0
         ncsc_replace_name(rst, "rst");                    // Port 1
         ncsc_replace_name(load_tx, "load_tx");            // Port 2
         ncsc_replace_name(load_tx2, "load_tx2");          // Port 3
         ncsc_replace_name(tx_start, "tx_start");          // Port 4
         ncsc_replace_name(tx_data, "tx_data");            // Port 5
         ncsc_replace_name(tx_parity, "tx_parity");        // Port 6
         ncsc_replace_name(tx_stop, "tx_stop");            // Port 7
         ncsc_replace_name(rx_start, "rx_start");          // Port 8
         ncsc_replace_name(rx_data, "rx_data");            // Port 9
         ncsc_replace_name(rx_parity, "rx_parity");        // Port 10
         ncsc_replace_name(rx_stop, "rx_stop");            // Port 11
         ncsc_replace_name(error_handle, "error_handle");  // Port 12
         ncsc_replace_name(rx_read, "rx_read");            // Port 13
         
         ncsc_replace_name(tx_buffer_full, "tx_buffer_full");  // Port 14
         ncsc_replace_name(rx_buffer_empty, "rx_buffer_empty");// Port 15
         ncsc_replace_name(parity_error, "parity_error");      // Port 16
         ncsc_replace_name(framing_error, "framing_error");    // Port 17
         ncsc_replace_name(overrun_error, "overrun_error");    // Port 18
         
         ncsc_replace_name(ctrl_parity_enabled, "ctrl_parity_enabled");// Port 19
         ncsc_replace_name(ctrl_parity_even, "ctrl_parity_even");      // Port 20
         ncsc_replace_name(ctrl_data_bits, "ctrl_data_bits");          // Port 21
         ncsc_replace_name(ctrl_stop_bits, "ctrl_stop_bits");          // Port 22
         
         ncsc_replace_name(rx_in, "rx_in");                // Port 23
         ncsc_replace_name(tx_out, "tx_out");              // Port 24
         ncsc_replace_name(data_in, "data_in");            // Port 25
         ncsc_replace_name(data_out, "data_out");          // Port 26
         ncsc_replace_name(addr, "addr");                  // Port 27
         
         ncsc_replace_name(dp_data_in, "dp_data_in");      // Port 28
         ncsc_replace_name(dp_addr, "dp_addr");            // Port 29
         ncsc_replace_name(dp_write_enable, "dp_write_enable");// Port 30
         
         ncsc_replace_name(start, "start");                // Port 31
         ncsc_replace_name(mem_we, "mem_we");              // Port 32
     }
 #endif
 };
 
 #endif