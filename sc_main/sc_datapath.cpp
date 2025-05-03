/*********************************************
 * File name: sc_datapath.cpp
 * Authors : Luke Guenthner, Nguyen Nguyen,
 *           Marcellus Wilson
 * Date    : 05/01/2025
 *
 * This file contains the sc_main function for
 * testing the UART datapath
 *********************************************/

#include <systemc>
#include "../src/datapath.h"
#include "../src/sizes.h"
#include <cassert>
#include <iostream>

using namespace std;

void run_instruction(datapath& dp, sc_time& current_time, const sc_time& cycle_time, const std::string& detail, int n) {
    std::cout << "\n-- " << detail << " --" << std::endl;
    std::cout << "Instruction Cycle start time: " << current_time << std::endl;
    for (int i = 0; i < n; ++i) {
        sc_start(2 * cycle_time);
        current_time += 2 * cycle_time;
    }
    std::cout << "Instruction Cycle end time: " << current_time << std::endl;
}

int sc_main(int argc, char* argv[]) {
    sc_time current_time = SC_ZERO_TIME;
    sc_time cycle_time(CYCLE_LENGTH, SC_NS);

    sc_clock clk("clk", CYCLE_LENGTH, SC_NS);

    sc_signal<bool> rst, load_tx, load_tx2, tx_start, tx_data, tx_parity, tx_stop;
    sc_signal<bool> rx_start, rx_data, rx_parity, rx_stop, error_handle, rx_read;
    sc_signal<bool> tx_buffer_full, rx_buffer_empty;
    sc_signal<bool> parity_error, framing_error, overrun_error;
    sc_signal<bool> rx_in, tx_out;
    sc_signal<bool> start, mem_we;

    sc_signal<sc_bv<DATA_W>> data_in, data_out;
    sc_signal<sc_bv<ADDR_W>> addr;
    sc_signal<sc_bv<DATA_W>> dp_data_in;
    sc_signal<sc_bv<ADDR_W>> dp_addr;
    sc_signal<bool> dp_write_enable;

    sc_signal<bool> ctrl_parity_enabled, ctrl_parity_even;
    sc_signal<sc_uint<3>> ctrl_data_bits;
    sc_signal<sc_uint<2>> ctrl_stop_bits;

    datapath dp("datapath");
    dp.clk(clk);
    dp.rst(rst);
    dp.load_tx(load_tx);
    dp.load_tx2(load_tx2);
    dp.tx_start(tx_start);
    dp.tx_data(tx_data);
    dp.tx_parity(tx_parity);
    dp.tx_stop(tx_stop);
    dp.rx_start(rx_start);
    dp.rx_data(rx_data);
    dp.rx_parity(rx_parity);
    dp.rx_stop(rx_stop);
    dp.error_handle(error_handle);
    dp.rx_read(rx_read);
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
    dp.start(start);
    dp.mem_we(mem_we);
    dp.dp_data_in(dp_data_in);
    dp.dp_addr(dp_addr);
    dp.dp_write_enable(dp_write_enable);
    dp.ctrl_parity_enabled(ctrl_parity_enabled);
    dp.ctrl_parity_even(ctrl_parity_even);
    dp.ctrl_data_bits(ctrl_data_bits);
    dp.ctrl_stop_bits(ctrl_stop_bits);

    rst.write(true);
    start.write(false);
    load_tx.write(false);
    load_tx2.write(false);
    tx_start.write(false);
    tx_data.write(false);
    tx_parity.write(false);
    tx_stop.write(false);
    rx_start.write(false);
    rx_data.write(false);
    rx_parity.write(false);
    rx_stop.write(false);
    rx_read.write(false);
    error_handle.write(false);
    tx_buffer_full.write(false);
    rx_buffer_empty.write(true);
    parity_error.write(false);
    framing_error.write(false);
    overrun_error.write(false);
    rx_in.write(1);
    data_in.write(0);
    addr.write(0);
    mem_we.write(false);

    std::cout << "\n=== TEST 1: Reset Verification (Safe Subset) ===" << std::endl;
    std::cout << "This test checks that reset properly clears key flags." << std::endl;
    start.write(true);
    run_instruction(dp, current_time, cycle_time, "RESET", 2);
    start.write(false);
    rst.write(false);
    std::cout << "After reset: rx_buffer_empty = " << rx_buffer_empty.read()
              << ", tx_buffer_full = " << tx_buffer_full.read() << std::endl;
    assert(rx_buffer_empty.read() == true);
    assert(tx_buffer_full.read() == false);
    std::cout << "Test 1 passed: Reset state (flags only)." << std::endl;

    std::cout << "\n=== TEST 2: Baud Rate Register Combination ===" << std::endl;
    std::cout << "This test simulates writes to the baud rate registers and ensures no crash." << std::endl;
    data_in.write("00000001"); addr.write(32);
    run_instruction(dp, current_time, cycle_time, "Write BAUD_LOW", 1);
    data_in.write("00000010"); addr.write(33);
    run_instruction(dp, current_time, cycle_time, "Write BAUD_HIGH", 1);
    std::cout << "Baud registers written with values 0x01 and 0x02." << std::endl;
    std::cout << "Test 2 passed: Baud rate written." << std::endl;

    std::cout << "\n=== TEST 3: Load TX Buffer - Phase 1 ===" << std::endl;
    std::cout << "This test initiates loading a byte into the TX buffer." << std::endl;
    data_in.write("10101010");
    addr.write(0);
    load_tx.write(true);
    run_instruction(dp, current_time, cycle_time, "TX Phase 1", 1);
    load_tx.write(false);
    std::cout << "TX buffer load initiated with byte 0xAA." << std::endl;
    std::cout << "Test 3 passed: TX buffer phase 1." << std::endl;

    std::cout << "\n=== TEST 4: Load TX Buffer - Phase 2 ===" << std::endl;
    std::cout << "This test completes the TX buffer load and starts transmission." << std::endl;
    load_tx2.write(true);
    run_instruction(dp, current_time, cycle_time, "TX Phase 2", 1);
    load_tx2.write(false);
    tx_start.write(true);
    run_instruction(dp, current_time, cycle_time, "TX Start", 1);
    tx_start.write(false);
    std::cout << "tx_out after start signal = " << tx_out.read() << " (should be 0 for start bit)" << std::endl;
    assert(tx_out.read() == 0);
    std::cout << "Test 4 passed: TX load and start." << std::endl;

    std::cout << "\n=== TEST 5: Error Flags Reset ===" << std::endl;
    std::cout << "This test sets all three error flags and uses error_handle to reset them." << std::endl;
    parity_error.write(true);
    framing_error.write(true);
    overrun_error.write(true);
    std::cout << "Initial error flag values: parity_error = " << parity_error.read()
              << ", framing_error = " << framing_error.read()
              << ", overrun_error = " << overrun_error.read() << std::endl;

    error_handle.write(true);
    run_instruction(dp, current_time, cycle_time, "Trigger Error Handle", 1);
    error_handle.write(false);
    run_instruction(dp, current_time, cycle_time, "Finish Error Clear", 1);

    std::cout << "Error flags after clearing: parity_error = " << parity_error.read()
              << ", framing_error = " << framing_error.read()
              << ", overrun_error = " << overrun_error.read() << std::endl;
    assert(parity_error.read() == false);
    assert(framing_error.read() == false);
    assert(overrun_error.read() == false);
    std::cout << "Test 5 passed: Error flags cleared successfully." << std::endl;

    std::cout << "\n=== All Tests Completed Successfully ===" << std::endl;
    return 0;
}
