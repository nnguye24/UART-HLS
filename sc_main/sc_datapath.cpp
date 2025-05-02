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

    run_instruction(dp, current_time, cycle_time, "RESET", 1);
    rst.write(false);

    cout << "\n--- TEST 1: TRANSMIT 0xA5 ---\n";
    data_in.write(0xA5);
    load_tx.write(true);
    run_instruction(dp, current_time, cycle_time, "load_tx", 1);
    load_tx.write(false);
    load_tx2.write(true);
    run_instruction(dp, current_time, cycle_time, "load_tx2", 1);
    load_tx2.write(false);
    tx_start.write(true);
    run_instruction(dp, current_time, cycle_time, "tx_start", 1);
    tx_start.write(false);
    tx_data.write(true);
    for (int i = 0; i < 8; ++i) run_instruction(dp, current_time, cycle_time, "tx_data", 1);
    tx_data.write(false);
    tx_stop.write(true);
    run_instruction(dp, current_time, cycle_time, "tx_stop", 1);
    tx_stop.write(false);
    assert(tx_out.read() == true && "TX line should return to idle after stop bit");
    cout << "TEST 1 passed\n";

    cout << "\n--- TEST 2: RECEIVE 0x3C ---\n";
    rx_read.write(true);
    run_instruction(dp, current_time, cycle_time, "clear RX buffer", 1);
    rx_read.write(false);
    rx_in.write(0);
    rx_start.write(true);
    run_instruction(dp, current_time, cycle_time, "rx_start", 1);
    rx_start.write(false);
    rx_data.write(true);
    for (int i = 0; i < 8; ++i) {
        rx_in.write((0x3C >> i) & 1);
        run_instruction(dp, current_time, cycle_time, "rx_data", 1);
    }
    rx_data.write(false);
    rx_parity.write(true);
    run_instruction(dp, current_time, cycle_time, "rx_parity", 1);
    rx_parity.write(false);
    rx_in.write(1);
    rx_stop.write(true);
    run_instruction(dp, current_time, cycle_time, "rx_stop", 1);
    rx_stop.write(false);
    run_instruction(dp, current_time, cycle_time, "commit", 1);
    assert(!parity_error.read() && "Should not flag parity error");
    cout << "TEST 2 passed\n";

    cout << "\n--- TEST 3: CONTINUOUS TX 0xA5 and 0x5A ---\n";
    data_in.write(0xA5);
    load_tx.write(true);
    run_instruction(dp, current_time, cycle_time, "load_tx1", 1);
    load_tx.write(false);
    load_tx2.write(true);
    run_instruction(dp, current_time, cycle_time, "load_tx2_1", 1);
    load_tx2.write(false);
    data_in.write(0x5A);
    load_tx.write(true);
    run_instruction(dp, current_time, cycle_time, "load_tx2", 1);
    load_tx.write(false);
    load_tx2.write(true);
    run_instruction(dp, current_time, cycle_time, "load_tx2_2", 1);
    load_tx2.write(false);
    tx_start.write(true);
    run_instruction(dp, current_time, cycle_time, "tx_start", 1);
    tx_start.write(false);
    tx_data.write(true);
    for (int i = 0; i < 8; ++i) run_instruction(dp, current_time, cycle_time, "tx1 data", 1);
    tx_data.write(false);
    tx_stop.write(true);
    run_instruction(dp, current_time, cycle_time, "tx1 stop", 1);
    tx_stop.write(false);
    tx_start.write(true);
    run_instruction(dp, current_time, cycle_time, "tx_start2", 1);
    tx_start.write(false);
    tx_data.write(true);
    for (int i = 0; i < 8; ++i) run_instruction(dp, current_time, cycle_time, "tx2 data", 1);
    tx_data.write(false);
    tx_stop.write(true);
    run_instruction(dp, current_time, cycle_time, "tx2 stop", 1);
    tx_stop.write(false);
    assert(tx_out.read() == true && "TX line should be idle after transmission");
    cout << "TEST 3 passed\n";

    cout << "\n--- TEST 5: RX BUFFER EMPTY FLAG ---\n";
    assert(!rx_buffer_empty.read() && "Buffer should contain data");
    rx_read.write(true);
    run_instruction(dp, current_time, cycle_time, "read out", 1);
    rx_read.write(false);
    assert(rx_buffer_empty.read() && "Buffer should now be empty");
    cout << "TEST 5 passed\n";

    cout << "\n--- TEST 6: RESET AFTER ACTIVITY ---\n";
    rst.write(true);
    run_instruction(dp, current_time, cycle_time, "reset", 1);
    rst.write(false);
    assert(tx_out.read() == true);
    assert(rx_buffer_empty.read() == true);
    assert(tx_buffer_full.read() == false);
    assert(!parity_error.read());
    assert(!framing_error.read());
    assert(!overrun_error.read());
    cout << "TEST 6 passed\n";

    cout << "\n--- TEST 7: RESET DURING TX LOAD ---\n";
    data_in.write(0xFF);
    load_tx.write(true);
    run_instruction(dp, current_time, cycle_time, "load_tx", 1);
    load_tx.write(false);
    load_tx2.write(true);
    run_instruction(dp, current_time, cycle_time, "load_tx2", 1);
    load_tx2.write(false);
    rst.write(true);
    run_instruction(dp, current_time, cycle_time, "reset mid-TX", 1);
    rst.write(false);
    assert(tx_out.read() == true);
    assert(tx_buffer_full.read() == false);
    cout << "TEST 7 passed\n";

    cout << "\n--- TEST 8: RX PARITY ERROR PREVENTS COMMIT ---\n";
    data_in.write(0);
    ctrl_parity_enabled.write(true);
    ctrl_parity_even.write(false);

    constexpr uint8_t PARITY_ERR_BYTE = 0x0F;
    rx_read.write(true);
    run_instruction(dp, current_time, cycle_time, "clear RX", 1);
    rx_read.write(false);
    rx_in.write(0);
    rx_start.write(true);
    run_instruction(dp, current_time, cycle_time, "rx_start", 1);
    rx_start.write(false);
    rx_data.write(true);
    for (int i = 0; i < 8; ++i) {
        rx_in.write((PARITY_ERR_BYTE >> i) & 1);
        run_instruction(dp, current_time, cycle_time, "rx_data", 1);
    }
    rx_data.write(false);
    rx_parity.write(true);
    rx_in.write(0);
    run_instruction(dp, current_time, cycle_time, "bad parity", 1);
    rx_parity.write(false);
    rx_stop.write(true);
    rx_in.write(1);
    run_instruction(dp, current_time, cycle_time, "rx_stop", 1);
    rx_stop.write(false);
    run_instruction(dp, current_time, cycle_time, "commit", 1);
    assert(parity_error.read() && "Parity error expected");
    assert(rx_buffer_empty.read() && "Should not commit corrupted byte");
    cout << "TEST 8 passed\n";

    cout << "\nAll UART datapath tests completed.\n";
    return 0;
}