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
        wait(period/2, SC_NS);
        clk.write(true);
        wait(period/2, SC_NS);
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

    // initialize
    rst.write(true);
    load_tx    .write(false);
    load_tx2   .write(false);
    tx_start   .write(false);
    tx_data    .write(false);
    tx_parity  .write(false);
    tx_stop    .write(false);
    rx_start   .write(false);
    rx_data    .write(false);
    rx_parity  .write(false);
    rx_stop    .write(false);
    rx_read    .write(false);
    error_handle       .write(false);
    tx_buffer_full    .write(false);
    rx_buffer_empty   .write(true);
    parity_error      .write(false);
    framing_error     .write(false);
    overrun_error     .write(false);
    rx_in             .write(1);     // idle = high
    data_in           .write(0);
    addr              .write(0);

    sc_start(cycle_time);
    rst.write(false);
    sc_time t = SC_ZERO_TIME;

    // ---------------------------------------------------
    // TEST 1: Basic Transmission of 0xA5
    // ---------------------------------------------------
    cout << "\n--- TEST 1: TRANSMIT 0xA5 ---\n";
    data_in.write(0xA5);
    load_tx.write(true);
    run_instruction(t, cycle_time, "load_tx pulse", 1);
    load_tx.write(false);

    load_tx2.write(true);
    run_instruction(t, cycle_time, "load_tx2 pulse", 1);
    load_tx2.write(false);

    tx_start.write(true);
    run_instruction(t, cycle_time, "tx_start pulse", 1);
    assert(tx_out.read() == false && "start bit must be 0");
    tx_start.write(false);

    tx_data.write(true);
    for (int i = 0; i < 8; ++i) {
        bool expected = ((0xA5 >> i) & 1);
        run_instruction(t, cycle_time, "tx_data bit", 1);
        assert(tx_out.read() == expected && "data bit mismatch");
    }
    tx_data.write(false);

    tx_parity.write(true);
    run_instruction(t, cycle_time, "tx_parity bit", 1);
    // parity disabled by default → output should remain high (idle)
    assert(tx_out.read() == true && "parity bit should be idle=1");
    tx_parity.write(false);

    tx_stop.write(true);
    run_instruction(t, cycle_time, "tx_stop bit", 1);
    assert(tx_out.read() == true && "stop bit must be 1");
    tx_stop.write(false);

    cout << "TEST 1 passed: TX sequence correct\n";

    // ---------------------------------------------------
    // TEST 2: Basic Reception of 0x3C
    // ---------------------------------------------------
    cout << "\n--- TEST 2: RECEIVE 0x3C ---\n";
    // clear empty flag
    rx_read.write(true);
    run_instruction(t, cycle_time, "rx_read pulse to clear", 1);
    rx_read.write(false);

    rx_in.write(0);
    rx_start.write(true);
    run_instruction(t, cycle_time, "rx_start pulse", 1);
    rx_start.write(false);

    constexpr uint8_t R = 0x3C;
    rx_data.write(true);
    for (int i = 0; i < 8; ++i) {
        rx_in.write((R >> i) & 1);
        run_instruction(t, cycle_time, "rx_data bit", 1);
    }
    rx_data.write(false);

    rx_parity.write(true);
    run_instruction(t, cycle_time, "rx_parity pulse", 1);
    rx_parity.write(false);
    // parity disabled → no error
    assert(!parity_error.read() && "parity_error should be false");

    rx_stop.write(true);
    rx_in.write(1);
    run_instruction(t, cycle_time, "rx_stop pulse", 1);
    rx_stop.write(false);
    run_instruction(t, cycle_time, "commit receive", 1);

    assert(data_out.read() == R && "received byte mismatch");
    assert(!framing_error.read() && "framing_error should be false");
    assert(!overrun_error.read() && "overrun_error should be false");
    cout << "TEST 2 passed: RX sequence correct\n";

    // ---------------------------------------------------
    // TEST 3: CONTINUOUS TRANSMISSION (0xA5 then 0x5A)
    // ---------------------------------------------------
    cout << "\n--- TEST 3: CONTINUOUS TX 0xA5 → 0x5A ---\n";
    // first byte
    data_in.write(0xA5);
    load_tx.write(true);
    run_instruction(t, cycle_time, "load_tx byte1", 1);
    load_tx.write(false);
    load_tx2.write(true);
    run_instruction(t, cycle_time, "load_tx2 byte1", 1);
    load_tx2.write(false);
    // second byte
    data_in.write(0x5A);
    load_tx.write(true);
    run_instruction(t, cycle_time, "load_tx byte2", 1);
    load_tx.write(false);
    load_tx2.write(true);
    run_instruction(t, cycle_time, "load_tx2 byte2", 1);
    load_tx2.write(false);

    // now send both start/data/stop bits back‐to‐back
    tx_start.write(true);
    run_instruction(t, cycle_time, "tx_start1", 1);
    assert(tx_out.read() == false);
    tx_start.write(false);
    tx_data.write(true);
    for (int i = 0; i < 8; ++i) run_instruction(t, cycle_time, "tx1 data", 1);
    tx_data.write(false);
    tx_stop.write(true);
    run_instruction(t, cycle_time, "tx1 stop", 1);
    tx_stop.write(false);

    tx_start.write(true);
    run_instruction(t, cycle_time, "tx_start2", 1);
    assert(tx_out.read() == false);
    tx_start.write(false);
    tx_data.write(true);
    for (int i = 0; i < 8; ++i) run_instruction(t, cycle_time, "tx2 data", 1);
    tx_data.write(false);
    tx_stop.write(true);
    run_instruction(t, cycle_time, "tx2 stop", 1);
    tx_stop.write(false);

    cout << "TEST 3 passed: continuous TX works\n";

    // ---------------------------------------------------
    // TEST 4: Framing‐Error Detection
    // ---------------------------------------------------
    cout << "\n--- TEST 4: FRAMING ERROR ---\n";
    rx_read.write(true);  run_instruction(t, cycle_time, "clear empty",1);  rx_read.write(false);
    rx_in.write(0);       rx_start.write(true);
    run_instruction(t, cycle_time, "start burst",1); rx_start.write(false);
    rx_data.write(true);
    for(int i=0; i<8; ++i) { rx_in.write((R>>i)&1); run_instruction(t, cycle_time, "data",1); }
    rx_data.write(false);
    rx_parity.write(true); run_instruction(t, cycle_time, "parity",1); rx_parity.write(false);
    // corrupt stop bit:
    rx_stop.write(true); rx_in.write(0);
    run_instruction(t, cycle_time, "bad stop",1);
    rx_stop.write(false);
    run_instruction(t, cycle_time, "commit framing",1);
    assert(framing_error.read() && "should flag framing_error");
    cout << "TEST 4 passed: framing error detected\n";

    // ---------------------------------------------------
    // TEST 5: Overrun‐Error Detection
    // ---------------------------------------------------
    cout << "\n--- TEST 5: OVERRUN ERROR ---\n";
    // first receive to fill buffer
    rx_read.write(true); run_instruction(t, cycle_time, "clear empty",1); rx_read.write(false);
    rx_in.write(0); rx_start.write(true);
    run_instruction(t, cycle_time, "start1",1); rx_start.write(false);
    rx_data.write(true);
    for(int i=0; i<8; ++i){ rx_in.write((R>>i)&1); run_instruction(t,cycle_time,"data1",1); }
    rx_data.write(false);
    rx_parity.write(true); run_instruction(t,cycle_time,"par1",1); rx_parity.write(false);
    rx_stop.write(true); rx_in.write(1); run_instruction(t,cycle_time,"stop1",1); rx_stop.write(false);
    run_instruction(t,cycle_time,"commit1",1);

    // second receive without clearing → overrun
    rx_in.write(0); rx_start.write(true);
    run_instruction(t, cycle_time, "start2",1); rx_start.write(false);
    rx_data.write(true);
    for(int i=0; i<8; ++i){ rx_in.write((R>>i)&1); run_instruction(t,cycle_time,"data2",1); }
    rx_data.write(false);
    rx_parity.write(true); run_instruction(t,cycle_time,"par2",1); rx_parity.write(false);
    rx_stop.write(true); rx_in.write(1); run_instruction(t,cycle_time,"stop2",1); rx_stop.write(false);
    run_instruction(t,cycle_time,"commit2",1);

    assert(overrun_error.read() && "should flag overrun_error");
    cout << "TEST 5 passed: overrun error detected\n";

    // ---------------------------------------------------
    // TEST 6: Buffer‐Empty Flag Toggles on rx_read
    // ---------------------------------------------------
    cout << "\n--- TEST 6: RX_BUFFER_EMPTY TOGGLE ---\n";
    // now buffer still full from previous, empty flag should be false
    assert(rx_buffer_empty.read() == false && "buffer should be non-empty");
    rx_read.write(true);
    run_instruction(t, cycle_time, "rx_read to pop byte",1);
    rx_read.write(false);
    assert(rx_buffer_empty.read() == true && "buffer should be empty after read");
    cout << "TEST 6 passed: buffer-empty flag toggles\n";

    cout << "\nAll datapath tests passed.\n";
    sc_close_vcd_trace_file(tf);
    return 0;
}