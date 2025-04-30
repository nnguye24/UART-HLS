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
#include <cassert>
#include <iostream>

// Clock generation function
void clock_gen(sc_signal<bool>& clk, int period) {
    while (true) {
        clk.write(false);
        wait(period / 2, SC_NS);
        clk.write(true);
        wait(period / 2, SC_NS);
    }
}

// Run “n” instruction cycles, printing a description
void run_instruction(sc_time& current_time,
                     const sc_time& cycle_time,
                     const std::string& detail,
                     int n = 1) {
    std::cout << detail
              << " @ " << current_time << std::endl;
    for (int i = 0; i < n; ++i) {
        sc_start(cycle_time);
        current_time += cycle_time;
    }
}

int sc_main(int argc, char* argv[]) {
    // === Signals ===
    sc_signal<bool> clk, rst;
    sc_signal<sc_uint<DATA_W>> data_in, data_out;
    sc_signal<sc_uint<ADDR_W>> addr;
    sc_signal<bool> chip_select, read_write, write_enable;

    // Datapath→Memory writes
    sc_signal<sc_uint<DATA_W>> dp_data_in, dp_data_out;
    sc_signal<sc_uint<ADDR_W>> dp_addr;
    sc_signal<bool> dp_write_enable;

    // Status inputs from UART
    sc_signal<bool> tx_buffer_full, rx_buffer_empty, error_indicator;

    // === DUT Instantiation ===
    memory_map mem("memory_map");
    mem.clk(clk);
    mem.rst(rst);
    mem.data_in(data_in);
    mem.data_out(data_out);
    mem.addr(addr);
    mem.chip_select(chip_select);
    mem.read_write(read_write);
    mem.write_enable(write_enable);
    mem.dp_data_in(dp_data_in);
    mem.dp_data_out(dp_data_out);
    mem.dp_addr(dp_addr);
    mem.dp_write_enable(dp_write_enable);
    mem.tx_buffer_full(tx_buffer_full);
    mem.rx_buffer_empty(rx_buffer_empty);
    mem.error_indicator(error_indicator);

    // === Trace ===
    sc_trace_file* tf = sc_create_vcd_trace_file("memory_map_trace");
    sc_trace(tf, clk, "clk");
    sc_trace(tf, rst, "rst");
    sc_trace(tf, data_in, "data_in");
    sc_trace(tf, data_out, "data_out");
    sc_trace(tf, addr, "addr");
    sc_trace(tf, chip_select, "chip_select");
    sc_trace(tf, read_write, "read_write");
    sc_trace(tf, write_enable, "write_enable");
    sc_trace(tf, dp_data_in, "dp_data_in");
    sc_trace(tf, dp_data_out, "dp_data_out");
    sc_trace(tf, dp_addr, "dp_addr");
    sc_trace(tf, dp_write_enable, "dp_write_enable");
    sc_trace(tf, tx_buffer_full, "tx_buffer_full");
    sc_trace(tf, rx_buffer_empty, "rx_buffer_empty");
    sc_trace(tf, error_indicator, "error_indicator");

    // === Clock & Timing ===
    const unsigned CYCLE_LENGTH = 10;             // ns
    sc_time cycle_time(CYCLE_LENGTH, SC_NS);
    sc_spawn(sc_bind(clock_gen, std::ref(clk), CYCLE_LENGTH));

    // === Initialize & Reset ===
    rst.write(true);
    data_in.write(0);
    addr.write(0);
    chip_select.write(false);
    read_write.write(false);
    write_enable.write(false);
    dp_data_in.write(0);
    dp_addr.write(0);
    dp_write_enable.write(false);
    tx_buffer_full.write(false);
    rx_buffer_empty.write(true);
    error_indicator.write(false);

    sc_start(cycle_time);   // one cycle with reset
    rst.write(false);

    sc_time t = SC_ZERO_TIME;

    // === TEST 1: Configuration Register Write/Read ===
    run_instruction(t, cycle_time, "TEST 1: Write LINE_CONTROL_REG", 1);
    data_in.write(0x5A);
    addr.write(LINE_CONTROL_REG);
    chip_select.write(true);
    read_write.write(true);
    write_enable.write(true);
    run_instruction(t, cycle_time, "  commit write", 1);
    write_enable.write(false);

    run_instruction(t, cycle_time, "  read back LINE_CONTROL_REG", 1);
    read_write.write(false);
    run_instruction(t, cycle_time, "  capture read", 1);
    assert(data_out.read() == 0x5A && "LINE_CONTROL_REG readback failed");
    chip_select.write(false);

    // === TEST 2: TX Buffer Write/Read via CPU ===
    run_instruction(t, cycle_time, "TEST 2: Write TX buffer[3]", 1);
    data_in.write(0xA5);
    addr.write(TX_BUFFER_START + 3);
    chip_select.write(true);
    read_write.write(true);
    write_enable.write(true);
    run_instruction(t, cycle_time, "  commit write", 1);
    write_enable.write(false);

    run_instruction(t, cycle_time, "  read back TX buffer[3]", 1);
    read_write.write(false);
    run_instruction(t, cycle_time, "  capture read", 1);
    assert(data_out.read() == 0xA5 && "TX buffer readback failed at index 3");
    chip_select.write(false);

    // === TEST 3: RX Buffer Write via Datapath, then CPU Read ===
    run_instruction(t, cycle_time, "TEST 3: Datapath writes RX buffer[5]", 1);
    dp_data_in.write(0xC3);
    dp_addr.write(RX_BUFFER_START + 5);
    dp_write_enable.write(true);
    run_instruction(t, cycle_time, "  commit dp write", 1);
    dp_write_enable.write(false);

    run_instruction(t, cycle_time, "  CPU reads RX buffer[5]", 1);
    chip_select.write(true);
    read_write.write(false);
    addr.write(RX_BUFFER_START + 5);
    run_instruction(t, cycle_time, "  capture read", 1);
    assert(data_out.read() == 0xC3 && "RX buffer readback failed at index 5");
    chip_select.write(false);

    // === TEST 4: Status Register Updates ===
    // No-error case
    run_instruction(t, cycle_time, "TEST 4a: Status no-error", 1);
    tx_buffer_full.write(true);
    rx_buffer_empty.write(false);
    error_indicator.write(false);
    run_instruction(t, cycle_time, "  capture status", 1);

    addr.write(LINE_STATUS_REG);
    chip_select.write(true);
    read_write.write(false);
    run_instruction(t, cycle_time, "  read LSR", 1);
    // Expect DATA_READY(0x01) only
    assert(data_out.read() == 0x01 && "LSR no-error check failed");
    chip_select.write(false);

    // Error case
    run_instruction(t, cycle_time, "TEST 4b: Status with error", 1);
    error_indicator.write(true);
    run_instruction(t, cycle_time, "  capture error status", 1);

    chip_select.write(true);
    run_instruction(t, cycle_time, "  read LSR", 1);
    // Expect DATA_READY + PARITY|FRAMING bits per your implementation
    // Here assuming LSR_PARITY_ERROR|LSR_FRAMING_ERROR = 0x0C plus DATA_READY = 0x01 => 0x0D
    assert(data_out.read() == 0x0D && "LSR error-indicator check failed");
    chip_select.write(false);

    // === Finish ===
    sc_close_vcd_trace_file(tf);
    std::cout << "UART Memory Map Simulation Complete" << std::endl;
    return 0;
}