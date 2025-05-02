/*********************************************
 * File name: sc_memory_map.cpp
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 5/01/2025
 *
 * This file contains the sc_main function for
 * testing the UART memory map, using C++ streams
 * and full two-phase stepping
 *********************************************/

#include "systemc.h"
#include "../src/memory_map.h"
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

// Advance simulation by n two-phase cycles with description
void run_instruction(sc_time& current_time,
                     const sc_time& cycle_time,
                     const string& detail,
                     int n = 1) {
    cout << ">>> " << detail
         << " @ time " << current_time
         << endl;
    for (int i = 0; i < n; ++i) {
        sc_start(2 * cycle_time);      // full compute+commit
        current_time += 2 * cycle_time;
    }
    cout << "<<< Completed: " << detail
         << endl << endl;
}

int sc_main(int argc, char* argv[]) {
    // === Signals ===
    sc_signal<bool> clk, rst;
    sc_signal<sc_uint<DATA_W>> data_in, data_out;
    sc_signal<sc_uint<ADDR_W>> addr;
    sc_signal<bool> chip_select, read_write, write_enable;
    sc_signal<sc_uint<DATA_W>> dp_data_in, dp_data_out;
    sc_signal<sc_uint<ADDR_W>> dp_addr;
    sc_signal<bool> dp_write_enable;
    sc_signal<bool> tx_buffer_full, rx_buffer_empty, error_indicator;

    // === Instantiate DUT ===
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

    // === Trace file ===
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

    // === Clock & timing ===
    const unsigned CYCLE_LENGTH = 10;  // ns
    sc_time cycle_time(CYCLE_LENGTH, SC_NS);
    sc_spawn(sc_bind(clock_gen, ref(clk), CYCLE_LENGTH));

    // === Initialization & reset ===
    cout << "=== INITIAL RESET SEQUENCE ===" << endl;
    rst.write(true);
    chip_select.write(false);
    read_write.write(false);
    write_enable.write(false);
    data_in.write(0);
    addr.write(0);
    dp_data_in.write(0);
    dp_addr.write(0);
    dp_write_enable.write(false);
    tx_buffer_full.write(false);
    rx_buffer_empty.write(true);
    error_indicator.write(false);

    sc_start(cycle_time);
    rst.write(false);
    sc_time t = SC_ZERO_TIME;

    // TEST 1: LINE_CONTROL_REG write/read
    cout << "\n--- TEST 1: LINE_CONTROL_REG WRITE/READ ---" << endl;
    chip_select.write(true);
    addr.write(LINE_CONTROL_REG);
    data_in.write(0x5A);
    read_write.write(true);
    write_enable.write(true);
    run_instruction(t, cycle_time, "Write 0x5A to LINE_CONTROL_REG", 1);
    write_enable.write(false);

    read_write.write(false);
    run_instruction(t, cycle_time, "Read back LINE_CONTROL_REG", 1);
    assert(data_out.read() == 0x5A);
    cout << "Result: LINE_CONTROL_REG == 0x5A" << endl;
    chip_select.write(false);

    // TEST 2: TX buffer CPU write/read
    cout << "\n--- TEST 2: TX BUFFER[3] CPU WRITE/READ ---" << endl;
    chip_select.write(true);
    addr.write(TX_BUFFER_START + 3);
    data_in.write(0xA5);
    read_write.write(true);
    write_enable.write(true);
    run_instruction(t, cycle_time, "Write 0xA5 to TX buffer[3]", 1);
    write_enable.write(false);

    read_write.write(false);
    run_instruction(t, cycle_time, "Read back TX buffer[3]", 1);
    assert(data_out.read() == 0xA5);
    cout << "Result: TX buffer[3] == 0xA5" << endl;
    chip_select.write(false);

    // TEST 3: RX buffer datapath write, CPU read
    cout << "\n--- TEST 3: RX BUFFER[5] DATAPATH WRITE / CPU READ ---" << endl;
    dp_data_in.write(0xC3);
    dp_addr.write(RX_BUFFER_START + 5);
    dp_write_enable.write(true);
    run_instruction(t, cycle_time, "Datapath writes 0xC3 to RX buffer[5]", 1);
    dp_write_enable.write(false);

    chip_select.write(true);
    addr.write(RX_BUFFER_START + 5);
    read_write.write(false);
    run_instruction(t, cycle_time, "CPU reads RX buffer[5]", 1);
    assert(data_out.read() == 0xC3);
    cout << "Result: RX buffer[5] == 0xC3" << endl;
    chip_select.write(false);

    // TEST 4: LINE_STATUS_REG no-error vs error
    cout << "\n--- TEST 4: LINE_STATUS_REG STATUS FLAGS ---" << endl;
    chip_select.write(true);
    addr.write(LINE_STATUS_REG);
    read_write.write(false);

    // 4a: no error (only DATA_READY)
    tx_buffer_full.write(true);
    rx_buffer_empty.write(false);
    error_indicator.write(false);
    run_instruction(t, cycle_time, "Read LSR with no errors", 1);
    assert(data_out.read() == LSR_DATA_READY);
    cout << "Result: LSR == DATA_READY only" << endl;

    // 4b: with error (PARITY+FRAMING)
    error_indicator.write(true);
    run_instruction(t, cycle_time, "Read LSR with error_indicator=true", 1);
    assert((data_out.read() & (LSR_PARITY_ERROR|LSR_FRAMING_ERROR)) != 0);
    cout << "Result: LSR includes PARITY and FRAMING errors" << endl;
    chip_select.write(false);

    // TEST 5: FIFO_CONTROL_REG & Scratch regs
    cout << "\n--- TEST 5: FIFO_CONTROL_REG & SCRATCH_REG WRITE/READ ---" << endl;
    chip_select.write(true);

    addr.write(FIFO_CONTROL_REG);
    data_in.write(0xAA);
    read_write.write(true);
    write_enable.write(true);
    run_instruction(t, cycle_time, "Write 0xAA to FIFO_CONTROL_REG", 1);
    write_enable.write(false);

    addr.write(SCRATCH_REG1);
    data_in.write(0x55);
    write_enable.write(true);
    run_instruction(t, cycle_time, "Write 0x55 to SCRATCH_REG1", 1);
    write_enable.write(false);

    addr.write(SCRATCH_REG2);
    data_in.write(0xFF);
    write_enable.write(true);
    run_instruction(t, cycle_time, "Write 0xFF to SCRATCH_REG2", 1);
    write_enable.write(false);

    read_write.write(false);
    addr.write(FIFO_CONTROL_REG);
    run_instruction(t, cycle_time, "Read back FIFO_CONTROL_REG", 1);
    assert(data_out.read() == 0xAA);
    cout << "Result: FIFO_CONTROL_REG == 0xAA" << endl;

    addr.write(SCRATCH_REG1);
    run_instruction(t, cycle_time, "Read back SCRATCH_REG1", 1);
    assert(data_out.read() == 0x55);
    cout << "Result: SCRATCH_REG1 == 0x55" << endl;

    addr.write(SCRATCH_REG2);
    run_instruction(t, cycle_time, "Read back SCRATCH_REG2", 1);
    assert(data_out.read() == 0xFF);
    cout << "Result: SCRATCH_REG2 == 0xFF" << endl;
    chip_select.write(false);

    // TEST 6: INVALID-ADDRESS HANDLING
    cout << "\n--- TEST 6: INVALID-ADDRESS HANDLING ---" << endl;
    chip_select.write(true);
    addr.write(RAM_SIZE + 5);
    data_in.write(0xDE);
    read_write.write(true);
    write_enable.write(true);
    run_instruction(t, cycle_time, "Attempt write to invalid address", 1);
    write_enable.write(false);

    read_write.write(false);
    run_instruction(t, cycle_time, "Read from invalid address", 1);
    assert(data_out.read() == 0xFF);
    cout << "Result: Invalid-address read returns 0xFF" << endl;
    chip_select.write(false);

    // TEST 7: FIFO_STATUS_REG basic flags
    cout << "\n--- TEST 7: FIFO_STATUS_REG TX_FULL / RX_EMPTY ---" << endl;
    chip_select.write(true);
    addr.write(FIFO_STATUS_REG);
    read_write.write(false);

    tx_buffer_full.write(false);
    rx_buffer_empty.write(true);
    run_instruction(t, cycle_time, "Read FSR with TX not full, RX empty", 1);
    assert(data_out.read() == FSR_RX_EMPTY);
    cout << "Result: FSR == RX_EMPTY only" << endl;

    tx_buffer_full.write(true);
    rx_buffer_empty.write(false);
    run_instruction(t, cycle_time, "Read FSR with TX full, RX not empty", 1);
    assert(data_out.read() == FSR_TX_FULL);
    cout << "Result: FSR == TX_FULL only" << endl;
    chip_select.write(false);

    // === Finish ===
    cout << "\nAll memory_map tests 1-7 passed successfully." << endl;
    sc_close_vcd_trace_file(tf);
    return 0;
}