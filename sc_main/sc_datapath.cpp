/*********************************************
 * File name: sc_datapath.cpp
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/22/2025
 *
 * This file contains the sc_main function for
 * testing the UART datapath
 *********************************************/

#include "systemc.h"
#include "../src/datapath.h"
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

// Test datapath module
int sc_main(int argc, char* argv[]) {
    // Create signals
    sc_signal<bool> clk, rst;
    sc_signal<bool> load_tx, tx_start, tx_data, tx_parity, tx_stop;
    sc_signal<bool> rx_start, rx_data, rx_parity, rx_stop, error_handle;
    sc_signal<bool> tx_buffer_full, rx_buffer_empty;
    sc_signal<bool> parity_error, framing_error, overrun_error;
    sc_signal<bool> rx_in, tx_out;
    sc_signal<sc_uint<DATA_W>> data_in, data_out;
    sc_signal<sc_uint<ADDR_W>> addr;
    
    // Instantiate datapath
    datapath dp("datapath");
    dp.clk(clk);
    dp.rst(rst);
    dp.load_tx(load_tx);
    dp.tx_start(tx_start);
    dp.tx_data(tx_data);
    dp.tx_parity(tx_parity);
    dp.tx_stop(tx_stop);
    dp.rx_start(rx_start);
    dp.rx_data(rx_data);
    dp.rx_parity(rx_parity);
    dp.rx_stop(rx_stop);
    dp.error_handle(error_handle);
    dp.tx_buffer_full(tx_buffer_full);
    dp.rx_buffer_empty(rx_buffer_empty);
    dp.parity_error(parity_error);
    dp.framing_error(framing_error);
    dp.overrun_error(overrun_error);
    dp.rx_in(rx_in);
    dp.tx_out(tx_out);
    dp.data_in(data_in);
    dp.data_out(data_out);
    dp.addr(addr);
    
    // Create trace file
    sc_trace_file *tf = sc_create_vcd_trace_file("datapath_trace");
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
    sc_trace(tf, rx_in, "rx_in");
    sc_trace(tf, tx_out, "tx_out");
    sc_trace(tf, data_in, "data_in");
    sc_trace(tf, data_out, "data_out");
    sc_trace(tf, addr, "addr");
    
    // Initialize signals
    rst.write(true);
    load_tx.write(false);
    tx_start.write(false);
    tx_data.write(false);
    tx_parity.write(false);
    tx_stop.write(false);
    rx_start.write(false);
    rx_data.write(false);
    rx_parity.write(false);
    rx_stop.write(false);
    error_handle.write(false);
    rx_in.write(1);  // Idle state is high
    data_in.write(0);
    addr.write(0);
    
    // Run simulation
    sc_start(10, SC_NS);  // Run with reset active
    rst.write(false);     // Release reset
    
    // Test transmit data
    data_in.write(0x55);  // Test data
    addr.write(0);        // TX buffer address
    load_tx.write(true);
    sc_start(10, SC_NS);
    load_tx.write(false);
    
    // Transmit sequence
    tx_start.write(true);
    sc_start(10, SC_NS);
    tx_start.write(false);
    tx_data.write(true);
    sc_start(80, SC_NS);  // 8 data bits
    tx_data.write(false);
    tx_parity.write(true);
    sc_start(10, SC_NS);
    tx_parity.write(false);
    tx_stop.write(true);
    sc_start(10, SC_NS);
    tx_stop.write(false);
    sc_start(20, SC_NS);
    
    // Test receive data
    // Simulate start bit
    rx_in.write(0);
    rx_start.write(true);
    sc_start(10, SC_NS);
    rx_start.write(false);
    
    // Simulate data bits (0xAA)
    rx_data.write(true);
    for (int i = 0; i < 8; i++) {
        rx_in.write((i % 2) ? 1 : 0);  // Alternating 1s and 0s
        sc_start(10, SC_NS);
    }
    rx_data.write(false);
    
    // Simulate parity bit
    rx_parity.write(true);
    rx_in.write(1);  // Even parity
    sc_start(10, SC_NS);
    rx_parity.write(false);
    
    // Simulate stop bit
    rx_stop.write(true);
    rx_in.write(1);
    sc_start(10, SC_NS);
    rx_stop.write(false);
    sc_start(20, SC_NS);
    
    // Close trace file
    sc_close_vcd_trace_file(tf);
    
    cout << "UART Datapath Simulation Complete" << endl;
    return 0;
}
