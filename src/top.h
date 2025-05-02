/**************************************************************
 * File Name: top.h
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/29/2025
 *
 * Contains the SystemC Module Header for the UART top-level module
 **************************************************************/

 #ifndef __TOP_H__
 #define __TOP_H__
 
 #include "systemc.h"
 #include "stratus_hls.h"
 #include "sizes.h"
 #include "datapath.h"
 #include "controller.h"
 #include "memory_map.h"
 SC_MODULE(top) {
   // Inputs from testbench
   sc_in<bool> clk;                        // Port 0
   sc_in<bool> rst;                        // Port 1
   sc_in<sc_uint<DATA_W>> data_in;         // Port 2
   sc_in<sc_uint<ADDR_W>> addr;            // Port 3
   sc_in<bool> chip_select;                // Port 4
   sc_in<bool> read_write;                 // Port 5
   sc_in<bool> write_enable;               // Port 6
   sc_in<bool> rx_in;                      // Port 7
 
   // Outputs to testbench
   sc_out<sc_uint<DATA_W>> data_out;       // Port 8
   sc_out<bool> tx_out;                    // Port 9
   sc_out<bool> tx_buffer_full;            // Port 10
   sc_out<bool> rx_buffer_empty;           // Port 11
   sc_out<bool> error_indicator;           // Port 12
 
   // Submodules
   datapath datapath_inst;
   controller controller_inst;
   memory_map memory_map_inst;
 
   // Internal signals for connecting modules
   
   // Controller to datapath signals
   sc_signal<bool> ctrl_to_dp_load_tx;
   sc_signal<bool> ctrl_to_dp_load_tx2;
   sc_signal<bool> ctrl_to_dp_tx_start;
   sc_signal<bool> ctrl_to_dp_tx_data;
   sc_signal<bool> ctrl_to_dp_tx_parity;
   sc_signal<bool> ctrl_to_dp_tx_stop;
   sc_signal<bool> ctrl_to_dp_rx_start;
   sc_signal<bool> ctrl_to_dp_rx_data;
   sc_signal<bool> ctrl_to_dp_rx_parity;
   sc_signal<bool> ctrl_to_dp_rx_stop;
   sc_signal<bool> ctrl_to_dp_error_handle;
   sc_signal<bool> ctrl_to_dp_rx_read;
   
   // Datapath to controller signals
   sc_signal<bool> dp_to_ctrl_tx_buffer_full;
   sc_signal<bool> dp_to_ctrl_rx_buffer_empty;
   sc_signal<bool> dp_to_ctrl_parity_error;
   sc_signal<bool> dp_to_ctrl_framing_error;
   sc_signal<bool> dp_to_ctrl_overrun_error;
   
   // Configuration signals
   sc_signal<bool> dp_to_ctrl_parity_enabled;
   sc_signal<bool> dp_to_ctrl_parity_even;
   sc_signal<sc_uint<3>> dp_to_ctrl_data_bits;
   sc_signal<sc_uint<2>> dp_to_ctrl_stop_bits;
   sc_signal<sc_uint<16>> dp_to_ctrl_baud_divisor;
   
   // Memory map to datapath signals
   sc_signal<sc_uint<DATA_W>> mem_to_dp_data;
   
   // Datapath to memory map signals
   sc_signal<sc_uint<DATA_W>> dp_to_mem_data;
   sc_signal<sc_uint<ADDR_W>> dp_to_mem_addr;
   sc_signal<bool> dp_to_mem_write_enable;
   
   // Internal signals for start and memory write enable
   sc_signal<bool> start_signal;
   sc_signal<bool> mem_we_signal;
   
   // Internal input values
   bool in_rst;
   sc_uint<DATA_W> in_data_in;
   sc_uint<ADDR_W> in_addr;
   bool in_chip_select;
   bool in_read_write;
   bool in_write_enable;
   bool in_rx_in;
   bool in_start;
   bool in_mem_we;
 
   // Top-level methods
   void process();
   void read_inputs();
   void write_outputs();
 
   // Test methods
   bool test_reset_datapath();
   bool test_reset_controller();
   bool test_reset_memory_map();
 
   SC_CTOR(top) : datapath_inst("datapath_inst"), 
                 controller_inst("controller_inst"), 
                 memory_map_inst("memory_map_inst") {
     SC_THREAD(process);
     sensitive << clk.pos();
     async_reset_signal_is(rst, false);
 
     // Connect all the Datapath Signals
     datapath_inst.clk(clk);
     datapath_inst.rst(rst);
     datapath_inst.load_tx(ctrl_to_dp_load_tx);
     datapath_inst.load_tx2(ctrl_to_dp_load_tx2);
     datapath_inst.tx_start(ctrl_to_dp_tx_start);
     datapath_inst.tx_data(ctrl_to_dp_tx_data);
     datapath_inst.tx_parity(ctrl_to_dp_tx_parity);
     datapath_inst.tx_stop(ctrl_to_dp_tx_stop);
     datapath_inst.rx_start(ctrl_to_dp_rx_start);
     datapath_inst.rx_data(ctrl_to_dp_rx_data);
     datapath_inst.rx_parity(ctrl_to_dp_rx_parity);
     datapath_inst.rx_stop(ctrl_to_dp_rx_stop);
     datapath_inst.error_handle(ctrl_to_dp_error_handle);
     datapath_inst.rx_read(ctrl_to_dp_rx_read);
     datapath_inst.tx_buffer_full(dp_to_ctrl_tx_buffer_full);
     datapath_inst.rx_buffer_empty(dp_to_ctrl_rx_buffer_empty);
     datapath_inst.parity_error(dp_to_ctrl_parity_error);
     datapath_inst.framing_error(dp_to_ctrl_framing_error);
     datapath_inst.overrun_error(dp_to_ctrl_overrun_error);
     datapath_inst.ctrl_parity_enabled(dp_to_ctrl_parity_enabled);
     datapath_inst.ctrl_parity_even(dp_to_ctrl_parity_even);
     datapath_inst.ctrl_data_bits(dp_to_ctrl_data_bits);
     datapath_inst.ctrl_stop_bits(dp_to_ctrl_stop_bits);
     datapath_inst.baud_divisor(dp_to_ctrl_baud_divisor);
     datapath_inst.rx_in(rx_in);
     datapath_inst.tx_out(tx_out);
     datapath_inst.data_in(mem_to_dp_data);
     datapath_inst.data_out(dp_to_mem_data);
     datapath_inst.addr(dp_to_mem_addr);
     datapath_inst.dp_data_in(dp_to_mem_data);
     datapath_inst.dp_addr(dp_to_mem_addr);
     datapath_inst.dp_write_enable(dp_to_mem_write_enable);
     datapath_inst.start(start_signal);
     datapath_inst.mem_we(mem_we_signal);
     
     // Connect all the Controller Signals
     controller_inst.clk(clk);
     controller_inst.rst(rst);
     controller_inst.start(start_signal);
     controller_inst.mem_we(mem_we_signal);
     controller_inst.tx_buffer_full(dp_to_ctrl_tx_buffer_full);
     controller_inst.rx_buffer_empty(dp_to_ctrl_rx_buffer_empty);
     controller_inst.rx_in(rx_in);
     controller_inst.parity_error(dp_to_ctrl_parity_error);
     controller_inst.framing_error(dp_to_ctrl_framing_error);
     controller_inst.overrun_error(dp_to_ctrl_overrun_error);
     controller_inst.parity_enabled(dp_to_ctrl_parity_enabled);
     controller_inst.parity_even(dp_to_ctrl_parity_even);
     controller_inst.data_bits(dp_to_ctrl_data_bits);
     controller_inst.stop_bits(dp_to_ctrl_stop_bits);
     controller_inst.baud_divisor(dp_to_ctrl_baud_divisor);
     controller_inst.load_tx(ctrl_to_dp_load_tx);
     controller_inst.load_tx2(ctrl_to_dp_load_tx2);
     controller_inst.tx_start(ctrl_to_dp_tx_start);
     controller_inst.tx_data(ctrl_to_dp_tx_data);
     controller_inst.tx_parity(ctrl_to_dp_tx_parity);
     controller_inst.tx_stop(ctrl_to_dp_tx_stop);
     controller_inst.rx_start(ctrl_to_dp_rx_start);
     controller_inst.rx_data(ctrl_to_dp_rx_data);
     controller_inst.rx_parity(ctrl_to_dp_rx_parity);
     controller_inst.rx_stop(ctrl_to_dp_rx_stop);
     controller_inst.rx_read(ctrl_to_dp_rx_read);
     controller_inst.error_handle(ctrl_to_dp_error_handle);
     
     // Connect all the Memory Map signals
     memory_map_inst.clk(clk);
     memory_map_inst.rst(rst);
     memory_map_inst.data_in(data_in);
     memory_map_inst.data_out(data_out);
     memory_map_inst.addr(addr);
     memory_map_inst.chip_select(chip_select);
     memory_map_inst.read_write(read_write);
     memory_map_inst.write_enable(write_enable);
     memory_map_inst.dp_data_out(mem_to_dp_data);
     memory_map_inst.dp_data_in(dp_to_mem_data);
     memory_map_inst.dp_addr(dp_to_mem_addr);
     memory_map_inst.dp_write_enable(dp_to_mem_write_enable);
     memory_map_inst.tx_buffer_full(dp_to_ctrl_tx_buffer_full);
     memory_map_inst.rx_buffer_empty(dp_to_ctrl_rx_buffer_empty);
     memory_map_inst.error_indicator(error_indicator);
   }
   
 #ifdef NC_SYSTEMC
  public:
   void ncsc_replace_names(){
     // Replace all the names using ncsc_replace_name for inputs and outputs
     
     // Inputs
     ncsc_replace_name(clk, "clk");                      // Port 0
     ncsc_replace_name(rst, "rst");                      // Port 1
     ncsc_replace_name(data_in, "data_in");              // Port 2
     ncsc_replace_name(addr, "addr");                    // Port 3
     ncsc_replace_name(chip_select, "chip_select");      // Port 4
     ncsc_replace_name(read_write, "read_write");        // Port 5
     ncsc_replace_name(write_enable, "write_enable");    // Port 6
     ncsc_replace_name(rx_in, "rx_in");                  // Port 7
     
     // Outputs
     ncsc_replace_name(data_out, "data_out");            // Port 8
     ncsc_replace_name(tx_out, "tx_out");                // Port 9
     ncsc_replace_name(tx_buffer_full, "tx_buffer_full");// Port 10
     ncsc_replace_name(rx_buffer_empty, "rx_buffer_empty");// Port 11
     ncsc_replace_name(error_indicator, "error_indicator");// Port 12
   }
 #endif
 };
 
 #endif