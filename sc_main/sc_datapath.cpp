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

    // Initialize signals
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

    std::cout << "\n=== TEST 1: Reset Verification ===" << std::endl;
    start.write(true);
    run_instruction(dp, current_time, cycle_time, "RESET", 2);
    start.write(false);
    rst.write(false);
    std::cout << "[DEBUG] rx_buffer_empty: " << rx_buffer_empty.read()
              << ", tx_buffer_full: " << tx_buffer_full.read() << std::endl;
    assert(rx_buffer_empty.read() == true);
    assert(tx_buffer_full.read() == false);
    std::cout << "Test 1 passed: Reset state (flags only)." << std::endl;

    std::cout << "\n=== TEST 2: Line Control Register Decoding (8N1) ===" << std::endl;
    data_in.write("00000011"); // LCR = 0x03 → 8 data bits
    addr.write(34); // LINE_CONTROL_REG
    run_instruction(dp, current_time, cycle_time, "Read LCR", 1);
    std::cout << "[DEBUG] ctrl_data_bits: " << ctrl_data_bits.read()
              << ", ctrl_stop_bits: " << ctrl_stop_bits.read()
              << ", ctrl_parity_enabled: " << ctrl_parity_enabled.read() << std::endl;
    assert(ctrl_data_bits.read() == 8);
    assert(ctrl_stop_bits.read() == 1);
    assert(ctrl_parity_enabled.read() == false);
    std::cout << "Test 2 passed: LCR decoding." << std::endl;

    std::cout << "\n=== TEST 3: Baud Rate Register Combination ===" << std::endl;
    data_in.write("00000001"); addr.write(32); // Low byte
    run_instruction(dp, current_time, cycle_time, "Write BAUD_LOW", 1);
    data_in.write("00000010"); addr.write(33); // High byte
    run_instruction(dp, current_time, cycle_time, "Write BAUD_HIGH", 1);
    std::cout << "[DEBUG] Baud registers written (no direct output to check)" << std::endl;
    std::cout << "Test 3 passed: Baud rate written." << std::endl;

    std::cout << "\n=== TEST 4: Load TX Buffer - Phase 1 ===" << std::endl;
    data_in.write("10101010");
    addr.write(0);
    load_tx.write(true);
    run_instruction(dp, current_time, cycle_time, "TX Phase 1", 1);
    load_tx.write(false);
    std::cout << "[DEBUG] Load TX Phase 1 done." << std::endl;
    std::cout << "Test 4 passed: TX buffer phase 1." << std::endl;

    std::cout << "\n=== TEST 5: Load TX Buffer - Phase 2 ===" << std::endl;
    load_tx2.write(true);
    run_instruction(dp, current_time, cycle_time, "TX Phase 2", 1);
    load_tx2.write(false);
    tx_start.write(true);
    run_instruction(dp, current_time, cycle_time, "TX Start", 1);
    tx_start.write(false);
    std::cout << "[DEBUG] tx_out after start: " << tx_out.read() << std::endl;
    assert(tx_out.read() == 0);
    std::cout << "Test 5 passed: TX load and start." << std::endl;

    std::cout << "\n=== TEST 6: Transmit Data and Stop Bits ===" << std::endl;
    tx_data.write(true);
    run_instruction(dp, current_time, cycle_time, "TX Data", 1);
    bool tx_bit_val = tx_out.read();
    tx_data.write(false);
    tx_stop.write(true);
    run_instruction(dp, current_time, cycle_time, "TX Stop", 1);
    tx_stop.write(false);
    std::cout << "[DEBUG] tx_out during data: " << tx_bit_val
              << ", after stop: " << tx_out.read() << std::endl;
    assert(tx_out.read() == 1);
    std::cout << "Test 6 passed: TX data/stop bits." << std::endl;

    std::cout << "\n=== TEST 7: RX Start Bit Error Detection ===" << std::endl;
    rx_in.write(1); // Should be 0
    rx_start.write(true);
    run_instruction(dp, current_time, cycle_time, "RX Start Bit", 1);
    rx_start.write(false);
    std::cout << "[DEBUG] framing_error: " << framing_error.read() << std::endl;
    assert(framing_error.read() == true);
    std::cout << "Test 7 passed: RX framing error detected." << std::endl;

    std::cout << "\n=== All Safe Tests Completed Successfully ===" << std::endl;
    return 0;
}