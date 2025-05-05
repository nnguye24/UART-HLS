#ifndef __MEMORY_MAP_H__
#define __MEMORY_MAP_H__

#include "systemc.h"
#include "stratus_hls.h"
#include "sizes.h"

// Memory map address definitions
#define TX_BUFFER_START     0   // 16 bytes (0-15)
#define RX_BUFFER_START    16   // 16 bytes (16-31)
#define CONFIG_REG_START   32   // 6 bytes (32-37)
#define STATUS_REG_START   38   // 2 bytes (38-39)

// Configuration register addresses
#define BAUD_RATE_LOW      32   // Baud rate divisor (low byte)
#define BAUD_RATE_HIGH     33   // Baud rate divisor (high byte)
#define LINE_CONTROL_REG   34   // Line control register (data bits, parity, stop bits)
#define FIFO_CONTROL_REG   35   // FIFO control register (enable, trigger levels)
#define SCRATCH_REG1       36   // Scratch register 1 (general purpose)
#define SCRATCH_REG2       37   // Scratch register 2 (general purpose)

// Status register addresses
#define LINE_STATUS_REG    38   // Line status register (errors, buffer status)
#define FIFO_STATUS_REG    39   // FIFO status register (buffer levels)

// Line control register bit definitions
#define LCR_DATA_BITS_MASK 0x03 // Bits 0-1: Data bits (0=5, 1=6, 2=7, 3=8)
#define LCR_STOP_BITS      0x04 // Bit 2: Stop bits (0=1, 1=2)
#define LCR_PARITY_ENABLE  0x08 // Bit 3: Parity enable
#define LCR_PARITY_EVEN    0x10 // Bit 4: Even parity when set, odd when clear
#define LCR_STICK_PARITY   0x20 // Bit 5: Stick parity
#define LCR_BREAK_CONTROL  0x40 // Bit 6: Break control
#define LCR_DLAB           0x80 // Bit 7: Divisor latch access bit

// Line status register bit definitions
#define LSR_DATA_READY     0x01 // Bit 0: Data ready
#define LSR_OVERRUN_ERROR  0x02 // Bit 1: Overrun error
#define LSR_PARITY_ERROR   0x04 // Bit 2: Parity error
#define LSR_FRAMING_ERROR  0x08 // Bit 3: Framing error
#define LSR_BREAK_DETECT   0x10 // Bit 4: Break detect
#define LSR_TX_EMPTY       0x20 // Bit 5: TX holding register empty
#define LSR_TX_IDLE        0x40 // Bit 6: TX shift register empty
#define LSR_RX_FIFO_ERROR  0x80 // Bit 7: RX FIFO error

// FIFO status register bit definitions
#define FSR_TX_FULL        0x01 // Bit 0: TX buffer full
#define FSR_RX_EMPTY       0x02 // Bit 1: RX buffer empty
#define FSR_TX_ALMOST_FULL 0x04 // Bit 2: TX buffer almost full
#define FSR_RX_ALMOST_EMPTY 0x08 // Bit 3: RX buffer almost empty
#define FSR_TX_COUNT_MASK  0x70 // Bits 4-6: TX buffer count (0-7)
#define FSR_RX_COUNT_MASK  0x80 // Bit 7: RX buffer count (0-7)
#define START_TX_BIT       0x10 // Bit 4: Start transmission bit

SC_MODULE(memory_map) {
    // Clock and reset
    sc_in<bool> clk;                        // Port 0
    sc_in<bool> rst;                        // Port 1
    
    // Memory interface
    sc_in<sc_bv<DATA_W>> data_in;         // Port 2
    sc_out<sc_bv<DATA_W>> data_out;       // Port 3
    sc_in<sc_bv<ADDR_W>> addr;            // Port 4
    sc_in<bool> chip_select;                // Port 5
    sc_in<bool> read_write;                 // Port 6
    sc_in<bool> write_enable;               // Port 7
    
    // Interface to datapath
    sc_out<sc_bv<DATA_W>> dp_data_out;    // Port 8
    sc_in<sc_bv<DATA_W>> dp_data_in;      // Port 9
    sc_in<sc_bv<ADDR_W>> dp_addr;         // Port 10
    sc_in<bool> dp_write_enable;            // Port 11
    
    // Status signals
    sc_in<bool> tx_buffer_full;             // Port 12 - Transmit Data
    sc_in<bool> rx_buffer_empty;            // Port 13 - Received
    sc_in<bool> error_indicator;            // Port 14
    
    // Memory array - single array for all memory
    sc_bv<DATA_W> Memory[RAM_SIZE];
    
    // Internal input values
    sc_bit in_rst;
    sc_bit in_chip_select;
    sc_bit in_read_write;
    sc_bit in_write_enable;
    sc_bv<DATA_W> in_data_in;
    sc_bv<ADDR_W> in_addr;
    sc_bv<DATA_W> in_dp_data_in;
    sc_bv<ADDR_W> in_dp_addr;
    sc_bit in_dp_write_enable;
    sc_bit in_tx_buffer_full;
    sc_bit in_rx_buffer_empty;
    sc_bit in_error_indicator;
    
    // Internal output values
    sc_bv<DATA_W> out_data_out;
    sc_bv<DATA_W> out_dp_data_out;
    
    // Methods
    void reset();
    void process();
    void read_inputs();
    void write_outputs();
    void compute();
    void commit();
    void update_status_registers();
    bool is_valid_address(sc_bv<ADDR_W> address);
    void clear_errors();
    
    // This section was intentionally removed as these helper methods
    // were not used anywhere in the codebase
    
    SC_CTOR(memory_map) {
        SC_THREAD(process);
        sensitive << clk.pos();
        async_reset_signal_is(rst, false);
    }
    
#ifdef NC_SYSTEMC
public:
    void ncsc_replace_names() {
        // Replace port names for simulation
        ncsc_replace_name(clk, "clk");                          // Port 0
        ncsc_replace_name(rst, "rst");                          // Port 1
        ncsc_replace_name(data_in, "data_in");                  // Port 2
        ncsc_replace_name(data_out, "data_out");                // Port 3
        ncsc_replace_name(addr, "addr");                        // Port 4
        ncsc_replace_name(chip_select, "chip_select");          // Port 5
        ncsc_replace_name(read_write, "read_write");            // Port 6
        ncsc_replace_name(write_enable, "write_enable");        // Port 7
        ncsc_replace_name(dp_data_out, "dp_data_out");          // Port 8
        ncsc_replace_name(dp_data_in, "dp_data_in");            // Port 9
        ncsc_replace_name(dp_addr, "dp_addr");                  // Port 10
        ncsc_replace_name(dp_write_enable, "dp_write_enable");  // Port 11
        ncsc_replace_name(tx_buffer_full, "tx_buffer_full");    // Port 12
        ncsc_replace_name(rx_buffer_empty, "rx_buffer_empty");  // Port 13
        ncsc_replace_name(error_indicator, "error_indicator");  // Port 14
    }
#endif

private:
    // Temporary values computed in compute()
    sc_bv<DATA_W> next_out_dout;
    sc_bv<4> next_out_io_out;
};

#endif
