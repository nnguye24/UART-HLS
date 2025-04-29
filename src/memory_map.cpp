/**************************************************************
 * File Name: memory_map.cpp
 * Authors: Luke Guenthner, Nguyen Nguyen, Marcellus Wilson
 * Date: 4/22/2025
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
 #define LINE_CONTROL_REG   34   // Line control register
 #define FIFO_CONTROL_REG   35   // FIFO control register
 #define SCRATCH_REG1       36   // Scratch register 1
 #define SCRATCH_REG2       37   // Scratch register 2
 
 // Status register addresses
 #define LINE_STATUS_REG    38   // Line status register
 #define FIFO_STATUS_REG    39   // FIFO status register
 
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
 #define LSR_TX_HOLDING_EMPTY 0x20 // Bit 5: TX holding register empty
 #define LSR_TX_EMPTY       0x40 // Bit 6: TX register empty
 #define LSR_RX_FIFO_ERROR  0x80 // Bit 7: RX FIFO error
 
 memory_map::memory_map() {
     // Initialize memory and outputs
     reset();
 }
 
 void memory_map::reset() {
     // Initialize all memory to zero
     for (int i = 0; i < RAM_SIZE; ++i) {
         Memory[i] = 0;
     }
     
     // Initialize outputs
     next_out_dout = 0;
     next_out_io_out = 0;
     data_out = 0;
     dp_data_out = 0;
     dp_addr = 0;
     dp_write_enable = false;
     
     // Initialize default configuration
     Memory[BAUD_RATE_LOW] = 0x03;     // Default baud rate divisor: 9600 baud
     Memory[BAUD_RATE_HIGH] = 0x00;    // (assuming 1.8432MHz clock)
     Memory[LINE_CONTROL_REG] = 0x03;  // 8 data bits, 1 stop bit, no parity
     Memory[FIFO_CONTROL_REG] = 0x01;  // Enable FIFOs
 }
 
 void memory_map::memory_access() {
     // Reset logic
     if (rst) {
         reset();
         return;
     }
     
     // Handle datapath memory access
     if (dp_write_enable) {
         if (dp_addr < RAM_SIZE) {
             Memory[dp_addr] = dp_data_in;
         }
     }
     
     // Memory access logic when chip is selected
     if (chip_select) {
         // Handle read operations
         if (!read_write) {
             // Read operation
             if (addr < RAM_SIZE) {
                 data_out = Memory[addr];
             } else {
                 // Invalid address - could set an error flag here
                 data_out = 0xFF;
             }
         }
         // Handle write operations
         else if (write_enable) {
             // Write operation
             if (addr < RAM_SIZE) {
                 Memory[addr] = data_in;
             }
         }
     }
     
     // Always update datapath output with current memory value
     dp_data_out = Memory[dp_addr];
 }
 
 void memory_map::status_update() {
     // Update line status register
     sc_uint<DATA_W> line_status = 0;
     
     // Set status bits based on UART state
     if (!rx_buffer_empty) {
         line_status |= LSR_DATA_READY;
     }
     
     if (tx_buffer_full) {
         // TX holding register is full
     } else {
         line_status |= LSR_TX_HOLDING_EMPTY;
     }
     
     // Set error bits
     if (error_indicator) {
         // This is a simplified approach - in a real design we would
         // have separate signals for each error type
         line_status |= LSR_PARITY_ERROR | LSR_FRAMING_ERROR;
     }
     
     // Update line status register
     Memory[LINE_STATUS_REG] = line_status;
     
     // Update FIFO status register (simplified)
     sc_uint<DATA_W> fifo_status = 0;
     
     // Bit 0: TX buffer full
     if (tx_buffer_full) {
         fifo_status |= 0x01;
     }
     
     // Bit 1: RX buffer empty
     if (rx_buffer_empty) {
         fifo_status |= 0x02;
     }
     
     // Update FIFO status register
     Memory[FIFO_STATUS_REG] = fifo_status;
 }
 
 // Helper methods for accessing specific memory regions

 
 void memory_map::compute() {
     // Memory access covers the compute phase
     memory_access();
 }
 
 void memory_map::commit() {
     // Status update happens in the commit phase
     status_update();
 }