/*********************************************
 * File name: top.cpp
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/29/2025
 *
 * This file contains the top module implementation for
 * the UART controller
 *********************************************/

 #include "top.h"

 void top::process() {
   // Initial reset
   {
     HLS_DEFINE_PROTOCOL("reset");
     
     // Each module has its own reset method
     // that will be called through the signal connections
   }
   
   {       
     HLS_DEFINE_PROTOCOL("wait");
     wait();
   }
 
   // Main processing loop
   while(true) {
     // Read inputs first
     {
       HLS_DEFINE_PROTOCOL("input");
       
       // We will come back here
       read_inputs();
     }
     
     // Processing phase
     {
       HLS_DEFINE_PROTOCOL("datapath_ops");
       
       // Allow modules to process
       // They will read from their input ports
       // The modules' process methods are triggered by clock edges
     }
     
     // Output phase
     {
       HLS_DEFINE_PROTOCOL("output");
       
       write_outputs();
     }
     
     // Wait for next cycle
     {       
       HLS_DEFINE_PROTOCOL("wait");
       wait();
     }
     
     // Additional wait for processing time
     {       
       HLS_DEFINE_PROTOCOL("wait");
       wait();
     }
   }
 }
 
 void top::read_inputs() {
   // Read inputs from external pins to internal variables
   in_rst = rst.read();
   in_data_in = data_in.read();
   in_addr = addr.read();
   in_chip_select = chip_select.read();
   in_read_write = read_write.read();
   in_write_enable = write_enable.read();
   in_rx_in = rx_in.read();
   
   // Set internal control signals
   start_signal.write(false);  // No external start in this implementation
   mem_we_signal.write(in_write_enable);
 }
 
 
 void top::write_outputs() {
   // Write external outputs
   data_out.write(memory_map_inst.data_out);
   tx_out.write(datapath_inst.tx_out);
   tx_buffer_full.write(datapath_inst.tx_buffer_full);
   rx_buffer_empty.write(datapath_inst.rx_buffer_empty);
   error_indicator.write(datapath_inst.parity_error || 
                         datapath_inst.framing_error || 
                         datapath_inst.overrun_error);
 }
 
 
 bool top::test_reset_datapath() {
   // Check if datapath registers are reset properly
   bool tx_reg_ok = (datapath_inst.tx_shift_register == 0);
   bool rx_reg_ok = (datapath_inst.rx_shift_register == 0);
   bool buffers_ok = (datapath_inst.tx_buf_head == 0) &&
                     (datapath_inst.tx_buf_tail == 0) &&
                     (datapath_inst.rx_buf_head == 0) &&
                     (datapath_inst.rx_buf_tail == 0);
   bool counters_ok = (datapath_inst.tx_bit_count == 0) &&
                      (datapath_inst.rx_bit_count == 0);
   bool signals_ok = (datapath_inst.out_tx_out == 1) &&
                     (datapath_inst.out_tx_buffer_full == false) &&
                     (datapath_inst.out_rx_buffer_empty == true);
   
   if(!tx_reg_ok) {
     std::cout << "datapath_inst.tx_shift_register - " << datapath_inst.tx_shift_register << std::endl;
     return false;
   }
   
   if(!rx_reg_ok) {
     std::cout << "datapath_inst.rx_shift_register - " << datapath_inst.rx_shift_register << std::endl;
     return false;
   }
   
   if(!buffers_ok) {
     std::cout << "Buffer pointers not reset properly" << std::endl;
     return false;
   }
   
   if(!counters_ok) {
     std::cout << "Bit counters not reset properly" << std::endl;
     return false;
   }
   
   if(!signals_ok) {
     std::cout << "Output signals not reset properly" << std::endl;
     return false;
   }
   
   return true;
 }
 
 bool top::test_reset_controller() {
   // Check if controller state is properly reset
   if(controller_inst.tx_state != TX_IDLE) {
     std::cout << "controller_inst.tx_state - " << controller_inst.tx_state << std::endl;
     return false;
   }
   
   if(controller_inst.rx_state != RX_IDLE) {
     std::cout << "controller_inst.rx_state - " << controller_inst.rx_state << std::endl;
     return false;
   }
   
   if(controller_inst.tx_next_state != TX_IDLE) {
     std::cout << "controller_inst.tx_next_state - " << controller_inst.tx_next_state << std::endl;
     return false;
   }
   
   if(controller_inst.rx_next_state != RX_IDLE) {
     std::cout << "controller_inst.rx_next_state - " << controller_inst.rx_next_state << std::endl;
     return false;
   }
   
   // Check all control signals are reset
   if(controller_inst.out_load_tx || 
      controller_inst.out_load_tx2 || 
      controller_inst.out_tx_start || 
      controller_inst.out_tx_data || 
      controller_inst.out_tx_parity || 
      controller_inst.out_tx_stop || 
      controller_inst.out_rx_start ||
      controller_inst.out_rx_data ||
      controller_inst.out_rx_parity ||
      controller_inst.out_rx_stop ||
      controller_inst.out_rx_read ||
      controller_inst.out_error_handle) {
     
     std::cout << "Controller outputs not reset properly" << std::endl;
     return false;
   }
   
   return true;
 }
 
 bool top::test_reset_memory_map() {
   // Verify memory map is properly reset
   for (int i = 0; i < RAM_SIZE; i++) {
     if (memory_map_inst.Memory[i] != 0) {
       std::cout << "Memory[" << i << "] is not reset, equals " << memory_map_inst.Memory[i] << std::endl;
       return false;
     }
   }
   
   return true;
 }