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

// Clock generation function
void clock_gen(sc_signal<bool>& clk, int period) {
    while (true) {
        clk.write(false);
        wait(period / 2, SC_NS);
        clk.write(true);
        wait(period / 2, SC_NS);
    }
}

// Advance simulation by n cycles with description
void run_instruction(sc_time& t,
                     const sc_time& cycle,
                     const string& detail,
                     int n = 1) {
    cout << ">>> " << detail << " @ " << t << endl;
    for (int i = 0; i < n; ++i) {
        sc_start(cycle);
        t += cycle;
    }
    cout << "<<< done: " << detail << "\n" << endl;
}

int sc_main(int argc, char* argv[]) {
    // === Signals ===
    sc_signal<bool> clk, rst;
    sc_signal<bool> load_tx, load_tx2, tx_start, tx_data, tx_parity, tx_stop;
    sc_signal<bool> rx_start, rx_data, rx_parity, rx_stop,
                  error_handle, rx_read;
    sc_signal<bool> tx_buffer_full, rx_buffer_empty;
    sc_signal<bool> parity_error, framing_error, overrun_error;
    sc_signal<bool> rx_in, tx_out;
    sc_signal<sc_uint<DATA_W>> data_in, data_out;
    sc_signal<sc_uint<ADDR_W>> addr;

    // === DUT Instantiation ===
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

    // === Clock & reset ===
    const unsigned CYCLE = 10; // ns
    sc_time cycle_time(CYCLE, SC_NS);
    sc_spawn(sc_bind(clock_gen, ref(clk), CYCLE));

    // === Initialize signals ===
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
    rx_in.write(1);  // idle = high
    data_in.write(0);
    addr.write(0);

    sc_start(cycle_time);
    rst.write(false);
    sc_time t = SC_ZERO_TIME;

    // === TEST 1: TRANSMIT 0xA5 ===
    cout << "\n--- TEST 1: TRANSMIT 0xA5 ---\n";
    data_in.write(0xA5);
    load_tx.write(true); run_instruction(t, cycle_time, "load_tx", 1); load_tx.write(false);
    load_tx2.write(true); run_instruction(t, cycle_time, "load_tx2", 1); load_tx2.write(false);
    tx_start.write(true); run_instruction(t, cycle_time, "tx_start", 1); tx_start.write(false);
    assert(tx_out.read() == false && "Start bit must be 0");
    tx_data.write(true);
    for (int i = 0; i < 8; ++i) {
        bool expected = (0xA5 >> i) & 1;
        run_instruction(t, cycle_time, "tx_data bit", 1);
        assert(tx_out.read() == expected && "Data bit mismatch");
    }
    tx_data.write(false);
    tx_parity.write(true); run_instruction(t, cycle_time, "tx_parity", 1); tx_parity.write(false);
    assert(tx_out.read() == true && "Parity bit should be 1 (idle)");
    tx_stop.write(true); run_instruction(t, cycle_time, "tx_stop", 1); tx_stop.write(false);
    assert(tx_out.read() == true && "Stop bit should be 1");
    cout << "TEST 1 passed\n";

    // === TEST 2: RECEIVE 0x3C ===
    cout << "\n--- TEST 2: RECEIVE 0x3C ---\n";
    rx_read.write(true); run_instruction(t, cycle_time, "clear RX buffer", 1); rx_read.write(false);
    rx_in.write(0); rx_start.write(true); run_instruction(t, cycle_time, "rx_start", 1); rx_start.write(false);
    constexpr uint8_t R = 0x3C;
    rx_data.write(true);
    for (int i = 0; i < 8; ++i) {
        rx_in.write((R >> i) & 1);
        run_instruction(t, cycle_time, "rx_data", 1);
    }
    rx_data.write(false);
    rx_parity.write(true); run_instruction(t, cycle_time, "rx_parity", 1); rx_parity.write(false);
    assert(!parity_error.read() && "Should not flag parity error");
    rx_in.write(1); rx_stop.write(true); run_instruction(t, cycle_time, "rx_stop", 1); rx_stop.write(false);
    run_instruction(t, cycle_time, "commit", 1);
    assert(data_out.read() == R && "Data mismatch");
    cout << "TEST 2 passed\n";

    // === TEST 3: CONTINUOUS TX ===
    cout << "\n--- TEST 3: CONTINUOUS TX 0xA5 and 0x5A ---\n";
    data_in.write(0xA5); load_tx.write(true); run_instruction(t, cycle_time, "load_tx1", 1); load_tx.write(false);
    load_tx2.write(true); run_instruction(t, cycle_time, "load_tx2_1", 1); load_tx2.write(false);
    data_in.write(0x5A); load_tx.write(true); run_instruction(t, cycle_time, "load_tx2", 1); load_tx.write(false);
    load_tx2.write(true); run_instruction(t, cycle_time, "load_tx2_2", 1); load_tx2.write(false);
    tx_start.write(true); run_instruction(t, cycle_time, "tx_start", 1); tx_start.write(false);
    tx_data.write(true); for (int i = 0; i < 8; ++i) run_instruction(t, cycle_time, "tx1 data", 1); tx_data.write(false);
    tx_stop.write(true); run_instruction(t, cycle_time, "tx1 stop", 1); tx_stop.write(false);
    tx_start.write(true); run_instruction(t, cycle_time, "tx_start2", 1); tx_start.write(false);
    tx_data.write(true); for (int i = 0; i < 8; ++i) run_instruction(t, cycle_time, "tx2 data", 1); tx_data.write(false);
    tx_stop.write(true); run_instruction(t, cycle_time, "tx2 stop", 1); tx_stop.write(false);
    cout << "TEST 3 passed\n";

    // === TEST 4: FRAMING ERROR ===
    cout << "\n--- TEST 4: FRAMING ERROR ---\n";
    rx_read.write(true); run_instruction(t, cycle_time, "clear RX", 1); rx_read.write(false);
    rx_in.write(0); rx_start.write(true); run_instruction(t, cycle_time, "rx_start", 1); rx_start.write(false);
    rx_data.write(true); for (int i = 0; i < 8; ++i) rx_in.write((R >> i) & 1), run_instruction(t, cycle_time, "rx_data", 1); rx_data.write(false);
    rx_parity.write(true); run_instruction(t, cycle_time, "rx_parity", 1); rx_parity.write(false);
    rx_in.write(0); rx_stop.write(true); run_instruction(t, cycle_time, "bad stop", 1); rx_stop.write(false);
    run_instruction(t, cycle_time, "commit", 1);
    assert(framing_error.read() && "Expected framing error");
    cout << "TEST 4 passed\n";

    // === TEST 5: RX BUFFER EMPTY FLAG ===
    cout << "\n--- TEST 5: RX BUFFER EMPTY FLAG ---\n";
    assert(!rx_buffer_empty.read() && "Buffer should contain data");
    rx_read.write(true); run_instruction(t, cycle_time, "read out", 1); rx_read.write(false);
    assert(rx_buffer_empty.read() && "Buffer should now be empty");
    cout << "TEST 5 passed\n";

    // === TEST 6: RESET SANITY CHECK ===
    cout << "\n--- TEST 6: RESET AFTER ACTIVITY ---\n";
    rst.write(true); run_instruction(t, cycle_time, "reset", 1); rst.write(false);
    assert(tx_out.read() == true);
    assert(rx_buffer_empty.read() == true);
    assert(tx_buffer_full.read() == false);
    assert(!parity_error.read());
    assert(!framing_error.read());
    assert(!overrun_error.read());
    cout << "TEST 6 passed\n";

    // === TEST 7: RESET DURING TX ===
    cout << "\n--- TEST 7: RESET DURING TX LOAD ---\n";
    data_in.write(0xFF);
    load_tx.write(true); run_instruction(t, cycle_time, "load_tx", 1); load_tx.write(false);
    load_tx2.write(true); run_instruction(t, cycle_time, "load_tx2", 1); load_tx2.write(false);
    rst.write(true); run_instruction(t, cycle_time, "reset mid-TX", 1); rst.write(false);
    assert(tx_out.read() == true);
    assert(tx_buffer_full.read() == false);
    cout << "TEST 7 passed\n";

    // === TEST 8: RX PARITY ERROR ===
    cout << "\n--- TEST 8: RX PARITY ERROR PREVENTS COMMIT ---\n";
    dp.out_ctrl_parity_enabled = true;
    dp.out_ctrl_parity_even = false;
    constexpr uint8_t PARITY_ERR_BYTE = 0x0F;
    rx_read.write(true); run_instruction(t, cycle_time, "clear RX", 1); rx_read.write(false);
    rx_in.write(0); rx_start.write(true); run_instruction(t, cycle_time, "rx_start", 1); rx_start.write(false);
    rx_data.write(true); for (int i = 0; i < 8; ++i) rx_in.write((PARITY_ERR_BYTE >> i) & 1), run_instruction(t, cycle_time, "rx_data", 1); rx_data.write(false);
    rx_parity.write(true); rx_in.write(0); run_instruction(t, cycle_time, "bad parity", 1); rx_parity.write(false);
    rx_stop.write(true); rx_in.write(1); run_instruction(t, cycle_time, "rx_stop", 1); rx_stop.write(false);
    run_instruction(t, cycle_time, "commit", 1);
    assert(parity_error.read() && "Parity error expected");
    assert(rx_buffer_empty.read() && "Should not commit corrupted byte");
    cout << "TEST 8 passed\n";

    cout << "\nAll UART datapath tests passed!\n";
    return 0;
}