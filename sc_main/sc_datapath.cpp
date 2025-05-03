/*********************************************
 * File name: sc_datapath.cpp
 * Authors : Luke Guenthner, Nguyen Nguyen,
 *           Marcellus Wilson
 * Date    : 05/01/2025
 *
 * This file contains the sc_main function for
 * testing the UART datapath
 *********************************************/

#include "systemc.h"
#include "../src/datapath.h"
#include "../src/sizes.h"
#include <cassert>
#include <iostream>

using namespace std;

void run_instruction(datapath& dp, sc_time& current_time, const sc_time& cycle_time, const std::string& detail, int n){
    std::cout << detail << std::endl;
    std::cout << "Instruction Cycle start time -" << current_time << std::endl;
    for(int i = 0; i < n; ++i){
        sc_start(2 * cycle_time);
        current_time += 2 * cycle_time;
    }
}

int sc_main(int argc, char* argv[]) {
    sc_time current_time = SC_ZERO_TIME;
    sc_time cycle_time(CYCLE_LENGTH, SC_NS);

    sc_clock clk("clk", CYCLE_LENGTH, SC_NS);
    sc_signal<bool> rst;
    sc_signal<bool> load_tx, load_tx2, tx_start, tx_data, tx_parity, tx_stop;
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
    sc_signal<bool> ctrl_parity_enabled;
    sc_signal<bool> ctrl_parity_even;
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
    run_instruction(dp, current_time, cycle_time, "RESET", 1);
    rst.write(false);

    cout << "\n--- TEST 1: CONFIGURATION LOAD CHECK ---\n";
    data_in.write(0x1F); // 8 data bits, 2 stop bits, parity enabled and even
    addr.write(34); // LINE_CONTROL_REG
    run_instruction(dp, current_time, cycle_time, "Set LCR", 1);
    run_instruction(dp, current_time, cycle_time, "Wait for config", 1);
    assert(ctrl_parity_enabled.read() == true);
    assert(ctrl_parity_even.read() == true);
    assert(ctrl_data_bits.read() == 8);
    assert(ctrl_stop_bits.read() == 2);
    cout << "TEST 1 passed\n";

    cout << "\n--- TEST 2: TX START ASSERTION CHECK ---\n";
    load_tx.write(true);
    run_instruction(dp, current_time, cycle_time, "load_tx phase 1", 1);
    load_tx.write(false);
    load_tx2.write(true);
    data_in.write(0xA5);
    run_instruction(dp, current_time, cycle_time, "load_tx2 phase 2", 1);
    load_tx2.write(false);
    tx_start.write(true);
    run_instruction(dp, current_time, cycle_time, "tx_start", 1);
    tx_start.write(false);
    run_instruction(dp, current_time, cycle_time, "post start", 1);
    assert(tx_out.read() == false);
    cout << "TEST 2 passed\n";

    cout << "\n--- TEST 3: TX DATA SHIFTING ---\n";
    tx_data.write(true);
    for (int i = 0; i < 8; ++i) run_instruction(dp, current_time, cycle_time, "shifting data", 1);
    tx_data.write(false);
    run_instruction(dp, current_time, cycle_time, "idle", 1);
    cout << "TEST 3 passed\n";

    cout << "\n--- TEST 4: TX STOP ASSERTION ---\n";
    tx_stop.write(true);
    run_instruction(dp, current_time, cycle_time, "tx_stop", 1);
    tx_stop.write(false);
    assert(tx_out.read() == true);
    cout << "TEST 4 passed\n";

    cout << "\nAll UART datapath tests completed.\n";
    return 0;
}
