/**************************************************************
 * File Name: memory_map.cpp
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/29/2025
 *
 * Memory map implementation for UART controller
 **************************************************************/

 #include "memory_map.h"
 #include <iostream>
 
 using namespace std;
 
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
 
 void memory_map::process() {
     {
         HLS_DEFINE_PROTOCOL("reset");
         reset();
     }
     
     {       
         HLS_DEFINE_PROTOCOL("wait");
         wait();
     }
     
     while(true) {
         // Read inputs
         {
             HLS_DEFINE_PROTOCOL("input");
             read_inputs();
         }
         
         // Compute memory operations
         {
             HLS_DEFINE_PROTOCOL("compute");
             compute();
         }
         
         // Update outputs and commit changes
         {
             HLS_DEFINE_PROTOCOL("output");
             commit();
             write_outputs();
         }
         
         // Wait for next cycle
         {       
             HLS_DEFINE_PROTOCOL("wait");
             wait();
         }
         
         // Additional wait for processing time
         {       
             HLS_DEFINE_PROTOCOL("wait");
             wait();
         }
     }
 }
 
 void memory_map::reset() {
     // Initialize all memory to zero
     for (int i = 0; i < RAM_SIZE; ++i) {
         Memory[i] = 0;
     }
     
     // Initialize default configuration registers
     Memory[BAUD_RATE_LOW] = 0x03;     // Default baud rate divisor: 9600 baud
     Memory[BAUD_RATE_HIGH] = 0x00;    // (assuming 1.8432MHz clock)
     Memory[LINE_CONTROL_REG] = 0x03;  // 8 data bits, 1 stop bit, no parity
     Memory[FIFO_CONTROL_REG] = 0x01;  // Enable FIFOs
     
     // Initialize output values
     out_data_out = 0;
     out_dp_data_out = 0;
 }
 
 void memory_map::read_inputs() {
     // Read all input signals
     in_rst = rst.read();

     in_data_in = data_in.read();
     in_addr = addr.read();

     in_chip_select = chip_select.read();

     in_read_write = read_write.read();
     in_write_enable = write_enable.read();
     in_dp_data_in = dp_data_in.read();
     in_dp_addr = dp_addr.read();
     in_dp_write_enable = dp_write_enable.read();

     in_tx_buffer_full = tx_buffer_full.read();
     in_rx_buffer_empty = rx_buffer_empty.read();
     in_error_indicator = error_indicator.read();
 }
 
 void memory_map::write_outputs() {
     // Update all output ports
     data_out.write(out_data_out);
     dp_data_out.write(out_dp_data_out);
 }
 
 void memory_map::compute() {
     // Reset logi
     if (in_rst) {
         reset();
         return;
     }
     
     // Handle datapath memory access (higher priority)
     if (in_dp_write_enable) {
         if (in_dp_addr < RAM_SIZE) {
             Memory[in_dp_addr] = in_dp_data_in;
         }
     }
     
     // CPU memory access
     if (in_chip_select) {
         if (!in_read_write) {
             // Read operation
             if (in_addr < RAM_SIZE) {
                 out_data_out = Memory[in_addr];
             } else {
                 // Invalid address
                 out_data_out = 0xFF;
             }
         } else if (in_write_enable) {
             // Write operation
             if (in_addr < RAM_SIZE) {
                 Memory[in_addr] = in_data_in;
             }
         }
     }
     
     // Always update datapath data output regardless of read/write operations
     if (in_dp_addr < RAM_SIZE) {
         out_dp_data_out = Memory[in_dp_addr];
     } else {
         out_dp_data_out = 0;
     }
 }
 
 void memory_map::commit() {
     // Update status registers based on UART state
     update_status_registers();
 }
 
 void memory_map::update_status_registers() {
     // Update line status register
     sc_uint<DATA_W> line_status = 0;
     
     // Set data ready flag if RX buffer not empty
     if (!in_rx_buffer_empty) {
         line_status |= LSR_DATA_READY;
     }
     
     // Set TX empty flag if TX buffer not full
     if (!in_tx_buffer_full) {
         line_status |= LSR_TX_EMPTY;
     }
     
     // Set TX idle flag (when transmitter is completely idle)
     if (!in_tx_buffer_full) {
         line_status |= LSR_TX_IDLE;
     }
     
     // Set error flags if error indicator is set
     // In a real implementation, we would have separate signals for each error type
     if (in_error_indicator) {
         line_status |= LSR_PARITY_ERROR | LSR_FRAMING_ERROR;
     }
     
     // Update line status register (read-only register)
     Memory[LINE_STATUS_REG] = line_status;
     
     // Update FIFO status register
     sc_uint<DATA_W> fifo_status = 0;
     
     // Set TX full flag
     if (in_tx_buffer_full) {
         fifo_status |= FSR_TX_FULL;
     }
     
     // Set RX empty flag
     if (in_rx_buffer_empty) {
         fifo_status |= FSR_RX_EMPTY;
     }
     
     // Update FIFO status register (read-only register)
     Memory[FIFO_STATUS_REG] = fifo_status;
 }
 
 // Helper methods for accessing specific memory regions
 sc_uint<DATA_W> memory_map::get_tx_buffer(unsigned int index) {
     if (index < TX_BUFFER_SIZE) {
         return Memory[TX_BUFFER_START + index];
     }
     return 0;
 }
 
 void memory_map::set_tx_buffer(unsigned int index, sc_uint<DATA_W> value) {
     if (index < TX_BUFFER_SIZE) {
         Memory[TX_BUFFER_START + index] = value;
     }
 }
 
 sc_uint<DATA_W> memory_map::get_rx_buffer(unsigned int index) {
     if (index < RX_BUFFER_SIZE) {
         return Memory[RX_BUFFER_START + index];
     }
     return 0;
 }
 
 void memory_map::set_rx_buffer(unsigned int index, sc_uint<DATA_W> value) {
     if (index < RX_BUFFER_SIZE) {
         Memory[RX_BUFFER_START + index] = value;
     }
 }
 
 // Helper methods for memory access
 bool memory_map::is_valid_address(sc_uint<ADDR_W> address) {
     return (address < RAM_SIZE);
 }
 
 void memory_map::clear_errors() {
     // Clear error bits in the line status register
     Memory[LINE_STATUS_REG] &= ~(LSR_PARITY_ERROR | LSR_FRAMING_ERROR | LSR_OVERRUN_ERROR);
 }