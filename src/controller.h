#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "systemc.h"
#include "stratus_hls.h"
#include "sizes.h"


SC_MODULE(controller) {
    // Clock and reset
    sc_in<bool> clk;                        // Port 0
    sc_in<bool> rst;                        // Port 1
    
    // Control signals
    sc_in<bool> start;                      // Port 2
    sc_in<bool> mem_we;                     // Port 3
    
    // Status inputs
    sc_in<bool> tx_buffer_full;             // Port 4
    sc_in<bool> rx_buffer_empty;            // Port 5
    sc_in<bool> rx_in;                      // Port 6 - Serial input line
    sc_in<bool> start_tx;                   // Port 26 - Start transmission signal
    
    // Error inputs
    sc_in<bool> parity_error;               // Port 7
    sc_in<bool> framing_error;              // Port 8
    sc_in<bool> overrun_error;              // Port 9
    
    // Configuration inputs
    sc_in<bool> parity_enabled;             // Port 10
    sc_in<bool> parity_even;                // Port 11
    sc_in<sc_uint<3>> data_bits;            // Port 12
    sc_in<sc_uint<2>> stop_bits;            // Port 13
    sc_in<sc_uint<16>> baud_divisor; 
    
    // Control outputs
    sc_out<bool> load_tx;                   // Port 14
    sc_out<bool> load_tx2;                  // Port 15
    sc_out<bool> tx_start;                  // Port 16
    sc_out<bool> tx_data;                   // Port 17
    sc_out<bool> tx_parity;                 // Port 18
    sc_out<bool> tx_stop;                   // Port 19
    sc_out<bool> rx_start;                  // Port 20
    sc_out<bool> rx_data;                   // Port 21
    sc_out<bool> rx_parity;                 // Port 22
    sc_out<bool> rx_stop;                   // Port 23
    sc_out<bool> rx_read;                   // Port 24
    sc_out<bool> error_handle;              // Port 25
    
    // FSM state registers
    sc_bv<4> tx_state;
    sc_bv<4> tx_next_state;
    sc_bv<4> rx_state;
    sc_bv<4> rx_next_state;
    
    // Internal registers and counters
    int tx_bit_counter;
    int rx_bit_counter;
    bool tx_parity_value;
    bool rx_parity_value;
    bool tx_done;
    bool rx_done;
    sc_uint<16> baud_counter;
    
    // Internal input values
    sc_bit in_start;
    sc_bit in_mem_we;
    sc_bit in_tx_buffer_full;
    sc_bit in_rx_buffer_empty;
    sc_bit in_rx_in;
    sc_bit in_parity_error;
    sc_bit in_framing_error;
    sc_bit in_overrun_error;
    sc_bit in_parity_enabled;
    sc_bit in_parity_even;
    sc_bit in_start_tx;
    sc_uint<3> in_data_bits;
    sc_uint<2> in_stop_bits;
    sc_uint<16> in_baud_divisor; 

    
    // Internal output values
    sc_bit out_load_tx;
    sc_bit out_load_tx2;
    sc_bit out_tx_start;
    sc_bit out_tx_data;
    sc_bit out_tx_parity;
    sc_bit out_tx_stop;
    sc_bit out_rx_start;
    sc_bit out_rx_data;
    sc_bit out_rx_parity;
    sc_bit out_rx_stop;
    sc_bit out_rx_read;
    sc_bit out_error_handle;
    
    // Methods
    void process();
    void reset_control_clear_regs();
    void clear_output_sc_bits();
    void read_inputs();
    void controller_fsm();
    void write_outputs();
    
    // Test method
    bool test_reset_controller();
    
    SC_CTOR(controller) {
        SC_THREAD(process);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);
    }
    
#ifdef NC_SYSTEMC
public:
    void ncsc_replace_names() {
        // Replace port names for simulation
        ncsc_replace_name(clk, "clk");                      // Port 0
        ncsc_replace_name(rst, "rst");                      // Port 1
        ncsc_replace_name(start, "start");                  // Port 2
        ncsc_replace_name(mem_we, "mem_we");                // Port 3
        ncsc_replace_name(tx_buffer_full, "tx_buffer_full");// Port 4
        ncsc_replace_name(rx_buffer_empty, "rx_buffer_empty");// Port 5
        ncsc_replace_name(rx_in, "rx_in");                  // Port 6
        ncsc_replace_name(parity_error, "parity_error");    // Port 7
        ncsc_replace_name(framing_error, "framing_error");  // Port 8
        ncsc_replace_name(overrun_error, "overrun_error");  // Port 9
        ncsc_replace_name(parity_enabled, "parity_enabled");// Port 10
        ncsc_replace_name(parity_even, "parity_even");      // Port 11
        ncsc_replace_name(data_bits, "data_bits");          // Port 12
        ncsc_replace_name(stop_bits, "stop_bits");          // Port 13
        ncsc_replace_name(load_tx, "load_tx");              // Port 14
        ncsc_replace_name(load_tx2, "load_tx2");            // Port 15
        ncsc_replace_name(tx_start, "tx_start");            // Port 16
        ncsc_replace_name(tx_data, "tx_data");              // Port 17
        ncsc_replace_name(tx_parity, "tx_parity");          // Port 18
        ncsc_replace_name(tx_stop, "tx_stop");              // Port 19
        ncsc_replace_name(rx_start, "rx_start");            // Port 20
        ncsc_replace_name(rx_data, "rx_data");              // Port 21
        ncsc_replace_name(rx_parity, "rx_parity");          // Port 22
        ncsc_replace_name(rx_stop, "rx_stop");              // Port 23
        ncsc_replace_name(rx_read, "rx_read");              // Port 24
        ncsc_replace_name(error_handle, "error_handle");    // Port 25
    }
#endif
};

#endif
