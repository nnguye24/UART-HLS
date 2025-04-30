/*********************************************
 * File name: sc_controller.cpp
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/22/2025
 *
 * This file contains the sc_main function for
 * testing the UART controller
 *********************************************/

#include "systemc.h"
#include "../src/controller.h"
#include "../src/sizes.h"
#include "assert.h"

// Clock generation function
void clock_gen(sc_signal<bool>& clk, int period) {
    while (true) {
        clk.write(false);
        wait(period / 2, SC_NS);
        clk.write(true);
        wait(period / 2, SC_NS);
    }
}

void run_instruction (controller& ctrl, sc_time& current_time, const sc_time& cycle_time, const std::string& detail, int n){
    std::cout << detail << std::endl;
    std::cout << "Instruction Cycle start time -" << current_time <<std::endl;
    for(int i = 0; i < n; ++ i){
        sc_start(2*cycle_time);
        current_time += 2 * cycle_time;
    }
}
// Test controller module
int sc_main(int argc, char* argv[]) {

    sc_time current_time = SC_ZERO_TIME;
    sc_time cycle_time(CYCLE_LENGTH, SC_NS);
    
    sc_clock clk("clk", CYCLE_LENGTH);
    // Create signals
    sc_signal<bool> clk, rst;
    sc_signal<bool> load_tx, tx_start, tx_data, tx_parity, tx_stop;
    sc_signal<bool> rx_start, rx_data, rx_parity, rx_stop, error_handle;
    sc_signal<bool> tx_buffer_full, rx_buffer_empty;
    sc_signal<bool> parity_error, framing_error, overrun_error;
    sc_signal<bool> start, mem_we, rx_in, parity_enabled, parity_even;
    sc_signal<bool> load_tx2, rx_read;
    //NEED Inputs
    sc_bv<DATA_W> data in;
    sc_bv<DATA_W> data_out;
    // Instantiate controller
    controller ctrl("controller");
    ctrl.clk(clk);
    ctrl.rst(rst);
    ctrl.start(start);
    ctrl.mem_we(mem_we);
    ctrl.load_tx(load_tx);
    ctrl.load_tx2(load_tx2);
    ctrl.tx_start(tx_start);
    ctrl.tx_data(tx_data);
    ctrl.tx_parity(tx_parity);
    ctrl.tx_stop(tx_stop);
    ctrl.rx_start(rx_start);
    ctrl.rx_data(rx_data);
    ctrl.rx_parity(rx_parity);
    ctrl.rx_stop(rx_stop);
    ctrl.rx_read(rx_read);
    ctrl.error_handle(error_handle);
    ctrl.tx_buffer_full(tx_buffer_full);
    ctrl.rx_buffer_empty(rx_buffer_empty);
    ctrl.rx_in(rx_in);
    ctrl.parity_error(parity_error);
    ctrl.framing_error(framing_error);
    ctrl.overrun_error(overrun_error);
    ctrl.data_in(data_in);
    ctrl.data_out(data_out);
    
    // Create trace file
    sc_trace_file *tf = sc_create_vcd_trace_file("controller_trace");
    sc_trace(tf, clk, "clk");
    sc_trace(tf, rst, "rst");
    sc_trace(tf,start, "start");
    sc_trace(tf,mem_we, "mem_we");
    sc_trace(tf, load_tx, "load_tx");
    sc_trace(tf, load_tx2, "load_tx2");
    sc_trace(tf, tx_start, "tx_start");
    sc_trace(tf, tx_data, "tx_data");
    sc_trace(tf, tx_parity, "tx_parity");
    sc_trace(tf, tx_stop, "tx_stop");
    sc_trace(tf, rx_start, "rx_start");
    sc_trace(tf, rx_data, "rx_data");
    sc_trace(tf, rx_parity, "rx_parity");
    sc_trace(tf, rx_stop, "rx_stop");
    sc_trace(tf,rx_read, "rx_read");
    sc_trace(tf, error_handle, "error_handle");
    sc_trace(tf, tx_buffer_full, "tx_buffer_full");
    sc_trace(tf, rx_buffer_empty, "rx_buffer_empty");
    sc_trace(tf, parity_error, "parity_error");
    sc_trace(tf, framing_error, "framing_error");
    sc_trace(tf, overrun_error, "overrun_error");
    
    // Initialize signals
    rst.write(true);
    tx_buffer_full.write(true)
    rx_buffer_empty.write(false);
    parity_error.write(true);
    framing_error.write(false);
    overrun_error.write(false);
    ctrl.reset_control_clear_regs(); 
    
    run_instruction(ctrl,current_time,cycle_time,"RESETTING INITIALIZE VARIABLES", 1);
    std::cout << "TEST 1: RESET" << std::endl;
    if(ctrl.test_reset_controller() == false)std::cout << "RESET FAILED" << std::endl;
    //Clock cycle
    
    run_instruction(ctrl, current_time, cycle_time, "TEST 2 testing starting states of TX & RX", 1); 
    std::cout << "TEST 2: FSM SEQUENCE" << std::endl; 
    mem_we.write(true);
    start.write(true);
    ctrl.controller_fsm();
    if(assert(tx_next_state == LOAD_TX2))std::cout  <<"TEST 2: FAILED mem_we[true] would cause FSM Logic to return" << std::endl;
    mem_we.write(false);
    start.write(false);
    run_instruction(ctrl, current_time,cycle_time, "TEST 3 FSM SEQUENCE", 1);
    std::cout << "TEST 3: FSM SEQUENCE | tx_buffer_full:true | rx_buffer_empty:true | parity_error:false | framing_error: false | overrun_error: false" << std::endl; 
    tx_buffer_full.write(true);
    rx_buffer_empty.write(true);
    rx_in.write(false);
    parity_error.write(false);
    framing_error.write(false);
    overrun_error.write(false);
    data_bits.write(8); 
    stop_bits.write(2);
    parity_enabled.write(false); 
    parity_even.write(false); 
    ctrl.read_inputs();
    ctrl.controller_fsm();
    if(!assert(tx_next_state == TX_IDLE && rx_buffer_empty == RX_IDLE))std::cout << "TEST 3: FAILED | check tx_next_state and rx_next_state" << std::endl;
    run_instruction(ctrl, current_time, cycle_time, "TEST 4 FSM SEQUENCE", 1); 
//Clock cycle
    std::cout << "TEST 4: FSM SEQUENCE" << std::endl; 
    tx_buffer_full.write(false);
    rx_in.write(true); 
    ctrl.read_inputs();
    ctrl.controller_fsm();
    if(!assert(tx_next_state == LOAD_TX2 && out_load_tx == true && out_rx_start == true && rx_next_state == RX_START_BIT)std::cout << "TEST 4: FAILED Output: tx_next_state:" << tx_next_state << " |out_load_tx: " << out_load_tx << std::endl;
    run_instruction(ctrl,current_time, cycle_time, "TEST 5: FSM SEQUENCE", 1);
    //Clock cycle
    std::cout << "TEST 5: FSM SEQUENCE" << std::endl;
    ctrl.controller_fsm();
    if(!assert(tx_next_state == TX_DATA_BITS && out_tx_start == true && tx_bit_counter == 0 && out_rx_start == true && rx_next_state == RX_DATA_BITS && rx_bit_counter = 0))std::cout << "TEST 5 FAILED" << std::endl;
    std::cout << "TEST 6: FSM SEQUENCE" << std::endl;
    ctrl.controller_fsm();
    if(!assert(tx_bit_counter == 1  && tx_next_state == TX_DATA_BITS)std::cout << "*TX LOGIC FAILED*" << std::endl;
    if(!assert(rx_bit_counter == 1 && rx_next_state == RX_DATA_BITS))std::cout <<"*RX LOGIC FAILED*" << std::endl;
    run_instruction(ctrl,current_time,cycle_time, "TEST 6: FSM SEQUENCE", 1);
    //Clock Cycle
    std::cout << "TEST 7: FSM SEQUENCE Skip iteration of data_bits" << std::endl;
    tx_bit_counter.write(8);
    rx_bit_counter.write(2);
    
    ctrl.controller_fsm();
    if(!assert(tx_next_state == TX_STOP_BIT))std::cout << "TX LOGIC FAILED | TEST 7" << std::endl;
    if(!assert(rx_next_state == RX_STOP_BIT))std::cout << "RX LOGIC FAILED | TEST 7" << std::endl; 
    run_instruction(ctrl, current_time, cycle_time, "TEST 7: FSM SEQUENCE", 1);
    //Clock Cycle
    std::cout << "TEST 8: FSM SEQUENCE" << std::endl; 
    ctrl.controller_fsm();
    if(!assert(out_tx_stop == true && tx_next_state == TX_IDLE && tx_done == false))std::cout << "TEST 8 TX LOGIC FAILED" << std::endl;
    if(!assert(rx_next_state == RX_IDLE && out_rx_read == true && rx_done == true))std::cout << "TEST 8 RX LOGIC FAILED" << std::endl;
    run_instruction(ctrl, current_time,cycle_time, "TEST 8: FSM SEQUENCE", 1);
    //Clock Cycle
    std::cout << "TEST 9: FSM SEQUENCE[All Data Reset][Errors Enabled]" << std::endl;
    ctrl.reset_control_clear_regs()
    start.write(false);
    mem_we.write(false);
    tx_buffer_full.write(false);
    rx_buffer_empty.write(true);
    rx_in.write(true);
    parity_error.write(true);
    framing_error.write(true); 
    overrun_error.write(true);
    parity_enabled.write(true);
    parity_even.write(true);
    data_bits.write(5);
    stop_bits.write(1); 
    ctrl.read_inputs();
    run_instruction(ctrl, current_time, cycle_time, "TEST 9: FSM_SEQUENCE", 1);
    //clock cycle
    ctrl.controller_fsm(); 
    if(!assert(out_load_tx == true && tx_new_state == LOAD_TX2))std::cout << "TEST 9: TX LOGIC FAILED" << std::endl;
    if(!assert(out_rx_start == true && rx_next_state == RX_START_BIT)) std::cout<< "TEST 9: RX LOGIC FAILED" << std:endl;
    run_instruction(ctrl, current_time,cycle_time, "TEST 10: FSM sequence",1);
    //Clock Cycle
    std::cout <<"TEST 10: FSM SEQUENCE" << std::endl;
    ctrl.controller_fsm(); 
    if(!assert(out_load_tx2 == true && tx_next_state == TX_START_BIT))std::cout << "TEST 10: TX LOGIC FAILED" << std::endl;
    if(!assert(rx_next_state ==  RX_DATA_BITS && rx_bit_counter == 0 && out_rx_start == true))std::cout << "TEST 10: RX LOGIC FAILED" << std::endl;
    run_instruction(ctrl, current_time, cycle_time, "TEST 11:FSM Sequence", 1);
    //clock cycle

    std::cout <<"TEST 11: FSM SEQUENCE" << std::endl;
    ctrl.controller_fsm();
    if(!assert(out_tx_start == true && tx_next_state == TX_DATA_BITS && tx_bit_counter == 0))std::cout << "TEST 11: TX Logic FAILED" << std::endl;
    if(!assert(rx_next_state == RX_PARITY_CHECK))std::cout << "TEST 11: RX LOGIC FAILED" << std::endl;
    run_instruction(ctrl, current_time, cycle_time, "TEST 12:FSM sequence",1);
//clock cycle
    std::cout <<"TEST 12: FSM SEQUENCE" << std::endl;
    ctrl.controller_fsm();
    if(!assert(tx_next_state == TX_PARITY_BIT)))std::cout << "TEST 12: TX LOGIC FAILED" << std::endl;

    if(!assert(out_rx_parity  == true && rx_next_state == ERROR_HANDLING))std::cout << "TEST 12: RX LOGIC FAILED" << std::endl;
    run_instruction(ctrl, current_time, cycle_time, "TEST 13: FSM SEQUENCE",1);
    //clock cycle

    std::cout <<"TEST 13: FSM SEQUENCE" << std::endl;
    ctrl.controller_fsm();
    if(!assert(out_tx_parity == true && tx_next_state == TX_STOP_BIT))std::cout << "TEST 13: TX LOGIC FAILED" << std::endl;

    if(!assert(out_error_handle = true && rx_next_state == RX_IDLE))std::cout << "TEST 13: RX LOGIC FAILED" << std::endl;
    run_instruction(ctrl, current_time, cycle_time, "14[FINAL TEST]", 1); 
    //clock cycle
    std::cout <<"TEST 14 [FINAL TESTING]" << std::endl; 

    if(!assert(out_tx_stop == true && tx_next_state == TX_IDLE && tx_done == true));
    run_instruction(ctrl, current_time, cycle_time, "END OF TESTING", 1);
    // Close trace file
    sc_close_vcd_trace_file(tf);
    
    std::cout << "UART Controller Simulation Complete" << std::endl;
    return 0;
}
