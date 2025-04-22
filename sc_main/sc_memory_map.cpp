/*********************************************
 * File name: sc_memory_map.cpp
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/22/2025
 *
 * This file contains the sc_main function for
 * testing the UART memory map
 *********************************************/

#include "systemc.h"
#include "../src/memory_map.h"
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

// Test memory_map module
int sc_main(int argc, char* argv[]) {
    // Create signals
    sc_signal<bool> clk, rst;
    sc_signal<sc_uint<DATA_W>> data_in, data_out;
    sc_signal<sc_uint<ADDR_W>> addr;
    sc_signal<bool> chip_select, read_write, write_enable;
    
    sc_signal<sc_uint<DATA_W>> dp_data_out, dp_data_in;
    sc_signal<sc_uint<ADDR_W>> dp_addr;
    sc_signal<bool> dp_write_enable;
    
    sc_signal<bool> tx_buffer_full, rx_buffer_empty, error_indicator;
    
    // Instantiate memory_map
    memory_map mem("memory_map");
    mem.clk(clk);
    mem.rst(rst);
    mem.data_in(data_in);
    mem.data_out(data_out);
    mem.addr(addr);
    mem.chip_select(chip_select);
    mem.read_write(read_write);
    mem.write_enable(write_enable);
    mem.dp_data_out(dp_data_out);
    mem.dp_data_in(dp_data_in);
    mem.dp_addr(dp_addr);
    mem.dp_write_enable(dp_write_enable);
    mem.tx_buffer_full(tx_buffer_full);
    mem.rx_buffer_empty(rx_buffer_empty);
    mem.error_indicator(error_indicator);
    
    // Create trace file
    sc_trace_file *tf = sc_create_vcd_trace_file("memory_map_trace");
    sc_trace(tf, clk, "clk");
    sc_trace(tf, rst, "rst");
    sc_trace(tf, data_in, "data_in");
    sc_trace(tf, data_out, "data_out");
    sc_trace(tf, addr, "addr");
    sc_trace(tf, chip_select, "chip_select");
    sc_trace(tf, read_write, "read_write");
    sc_trace(tf, write_enable, "write_enable");
    sc_trace(tf, dp_data_out, "dp_data_out");
    sc_trace(tf, dp_data_in, "dp_data_in");
    sc_trace(tf, dp_addr, "dp_addr");
    sc_trace(tf, dp_write_enable, "dp_write_enable");
    sc_trace(tf, tx_buffer_full, "tx_buffer_full");
    sc_trace(tf, rx_buffer_empty, "rx_buffer_empty");
    sc_trace(tf, error_indicator, "error_indicator");
    
    // Initialize signals
    rst.write(true);
    data_in.write(0);
    addr.write(0);
    chip_select.write(false);
    read_write.write(false);
    write_enable.write(false);
    dp_data_in.write(0);
    tx_buffer_full.write(false);
    rx_buffer_empty.write(true);
    error_indicator.write(false);
    
    // Run simulation
    sc_start(10, SC_NS);  // Run with reset active
    rst.write(false);     // Release reset
    
    // Test write operation to config register
    data_in.write(0xAA);  // Test data
    addr.write(16);       // Config register address
    chip_select.write(true);
    read_write.write(true);  // Write operation
    write_enable.write(true);
    sc_start(10, SC_NS);
    
    // Test read operation from config register
    addr.write(16);       // Config register address
    read_write.write(false);  // Read operation
    sc_start(10, SC_NS);
    
    // Test write operation to transmit buffer
    data_in.write(0x55);  // Test data
    addr.write(0);        // TX buffer address
    read_write.write(true);  // Write operation
    sc_start(10, SC_NS);
    
    // Test read operation from receive buffer
    addr.write(8);        // RX buffer address
    read_write.write(false);  // Read operation
    sc_start(10, SC_NS);
    
    // Test status register updates
    tx_buffer_full.write(true);
    sc_start(10, SC_NS);
    rx_buffer_empty.write(false);
    sc_start(10, SC_NS);
    error_indicator.write(true);
    sc_start(10, SC_NS);
    
    // Read status register
    addr.write(32);       // Status register address
    sc_start(10, SC_NS);
    
    // Cleanup
    chip_select.write(false);
    sc_start(10, SC_NS);
    
    // Close trace file
    sc_close_vcd_trace_file(tf);
    
    cout << "UART Memory Map Simulation Complete" << endl;
    return 0;
}
