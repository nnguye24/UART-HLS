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

// Clock generation function
void clock_gen(sc_signal<bool>& clk, int period) {
    while (true) {
        clk.write(false);
        wait(period / 2, SC_NS);
        clk.write(true);
        wait(period / 2, SC_NS);
    }
}

// Test controller module
int sc_main(int argc, char* argv[]) {
    // Create signals
    sc_signal<bool> clk, rst;
    sc_signal<bool> load_tx, tx_start, tx_data, tx_parity, tx_stop;
    sc_signal<bool> rx_start, rx_data, rx_parity, rx_stop, error_handle;
    sc_signal<bool> tx_buffer_full, rx_buffer_empty;
    sc_signal<bool> parity_error, framing_error, overrun_error;
    
    // Instantiate controller
    controller ctrl("controller");
    ctrl.clk(clk);
    ctrl.rst(rst);
    ctrl.load_tx(load_tx);
    ctrl.tx_start(tx_start);
    ctrl.tx_data(tx_data);
    ctrl.tx_parity(tx_parity);
    ctrl.tx_stop(tx_stop);
    ctrl.rx_start(rx_start);
    ctrl.rx_data(rx_data);
    ctrl.rx_parity(rx_parity);
    ctrl.rx_stop(rx_stop);
    ctrl.error_handle(error_handle);
    ctrl.tx_buffer_full(tx_buffer_full);
    ctrl.rx_buffer_empty(rx_buffer_empty);
    ctrl.parity_error(parity_error);
    ctrl.framing_error(framing_error);
    ctrl.overrun_error(overrun_error);
    
    // Create trace file
    sc_trace_file *tf = sc_create_vcd_trace_file("controller_trace");
    sc_trace(tf, clk, "clk");
    sc_trace(tf, rst, "rst");
    sc_trace(tf, load_tx, "load_tx");
    sc_trace(tf, tx_start, "tx_start");
    sc_trace(tf, tx_data, "tx_data");
    sc_trace(tf, tx_parity, "tx_parity");
    sc_trace(tf, tx_stop, "tx_stop");
    sc_trace(tf, rx_start, "rx_start");
    sc_trace(tf, rx_data, "rx_data");
    sc_trace(tf, rx_parity, "rx_parity");
    sc_trace(tf, rx_stop, "rx_stop");
    sc_trace(tf, error_handle, "error_handle");
    sc_trace(tf, tx_buffer_full, "tx_buffer_full");
    sc_trace(tf, rx_buffer_empty, "rx_buffer_empty");
    sc_trace(tf, parity_error, "parity_error");
    sc_trace(tf, framing_error, "framing_error");
    sc_trace(tf, overrun_error, "overrun_error");
    
    // Initialize signals
    rst.write(true);
    tx_buffer_full.write(false);
    rx_buffer_empty.write(true);
    parity_error.write(false);
    framing_error.write(false);
    overrun_error.write(false);
    
    // Run simulation
    sc_start(10, SC_NS);  // Run with reset active
    rst.write(false);     // Release reset
    
    // Test transmit sequence
    tx_buffer_full.write(true);
    sc_start(20, SC_NS);
    tx_buffer_full.write(false);
    sc_start(100, SC_NS);
    
    // Test receive sequence
    rx_buffer_empty.write(false);
    sc_start(20, SC_NS);
    rx_buffer_empty.write(true);
    sc_start(100, SC_NS);
    
    // Test error handling
    parity_error.write(true);
    sc_start(20, SC_NS);
    parity_error.write(false);
    sc_start(100, SC_NS);
    
    // Close trace file
    sc_close_vcd_trace_file(tf);
    
    cout << "UART Controller Simulation Complete" << endl;
    return 0;
}
