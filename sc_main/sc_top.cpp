/*********************************************
 * File name: sc_top.cpp
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/22/2025
 *
 * This file contains the sc_main function for
 * testing the UART top-level module
 *********************************************/

#include "systemc.h"
#include "../src/top.h"
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

// Test top module
int sc_main(int argc, char* argv[]) {
    // Create signals
    sc_signal<bool> clk, rst;
    sc_signal<sc_uint<DATA_W>> data_in, data_out;
    sc_signal<sc_uint<ADDR_W>> addr;
    sc_signal<bool> chip_select, read_write, write_enable;
    sc_signal<bool> rx_in, tx_out;
    sc_signal<bool> tx_buffer_full, rx_buffer_empty, error_indicator;
    
    // Instantiate top module
    top uart_top("uart_top");
    uart_top.clk(clk);
    uart_top.rst(rst);
    uart_top.data_in(data_in);
    uart_top.data_out(data_out);
    uart_top.addr(addr);
    uart_top.chip_select(chip_select);
    uart_top.read_write(read_write);
    uart_top.write_enable(write_enable);
    uart_top.rx_in(rx_in);
    uart_top.tx_out(tx_out);
    uart_top.tx_buffer_full(tx_buffer_full);
    uart_top.rx_buffer_empty(rx_buffer_empty);
    uart_top.error_indicator(error_indicator);
    
    // Create a process for clock generation
    sc_clock system_clk("system_clk", CYCLE_LENGTH, SC_NS);
    clk(system_clk);
    
    // Create trace file
    sc_trace_file *tf = sc_create_vcd_trace_file("uart_top_trace");
    sc_trace(tf, clk, "clk");
    sc_trace(tf, rst, "rst");
    sc_trace(tf, data_in, "data_in");
    sc_trace(tf, data_out, "data_out");
    sc_trace(tf, addr, "addr");
    sc_trace(tf, chip_select, "chip_select");
    sc_trace(tf, read_write, "read_write");
    sc_trace(tf, write_enable, "write_enable");
    sc_trace(tf, rx_in, "rx_in");
    sc_trace(tf, tx_out, "tx_out");
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
    rx_in.write(1);  // Idle state is high
    
    // Run simulation
    sc_start(20, SC_NS);  // Run with reset active
    rst.write(false);     // Release reset
    sc_start(20, SC_NS);
    
    // Test 1: Configure UART
    // Configure baud rate
    chip_select.write(true);
    read_write.write(true);  // Write operation
    write_enable.write(true);
    
    // Write to baud rate divisor registers
    addr.write(16);  // Address of first config register
    data_in.write(0x03);  // Set baud rate divisor (low byte)
    sc_start(10, SC_NS);
    
    addr.write(17);  // Address of second config register
    data_in.write(0x00);  // Set baud rate divisor (high byte)
    sc_start(10, SC_NS);
    
    // Write to line control register
    addr.write(18);  // Line control register
    data_in.write(0x03);  // 8 data bits, no parity, 1 stop bit
    sc_start(10, SC_NS);
    
    // Test 2: Write data to transmit buffer
    addr.write(0);  // TX buffer address
    data_in.write(0x55);  // Data to transmit ('U')
    sc_start(10, SC_NS);
    
    // Test 3: Read status register
    read_write.write(false);  // Read operation
    addr.write(32);  // Status register address
    sc_start(10, SC_NS);
    
    // Test 4: Wait for transmission to complete
    chip_select.write(false);
    sc_start(200, SC_NS);
    
    // Test 5: Receive data
    // Simulate UART receive sequence (start bit, 8 data bits, stop bit)
    // Start bit
    rx_in.write(0);
    sc_start(10, SC_NS);
    
    // Data bits (0xAA)
    for (int i = 0; i < 8; i++) {
        rx_in.write((i % 2) ? 1 : 0);  // Alternating 1s and 0s
        sc_start(10, SC_NS);
    }
    
    // Stop bit
    rx_in.write(1);
    sc_start(10, SC_NS);
    sc_start(50, SC_NS);
    
    // Test 6: Read received data
    chip_select.write(true);
    read_write.write(false);  // Read operation
    addr.write(8);  // RX buffer address
    sc_start(10, SC_NS);
    
    // Finish simulation
    chip_select.write(false);
    sc_start(50, SC_NS);
    
    // Close trace file
    sc_close_vcd_trace_file(tf);
    
    cout << "UART Controller Simulation Complete" << endl;
    return 0;
}
