# **UART Controller Project Proposal**

Group Members:  
Luke Guenthner [lguenth2@nd.edu](mailto:lguenth2@nd.edu)  
Nguyen Nguyen [nnguye24@nd.edu](mailto:nnguye24@nd.edu)  
Marcellus Wilson [mwilso36@nd.edu](mailto:mwilso36@nd.edu)

## **Table of Contents**

1. [Executive Summary](#1.-executive-summary)  
2. [Project Description](#2.-project-description)   
* [2.1 Overview and Purpose](#2.1-overview-and-purpose)   
* [2.2 Key Features](#2.2-key-features)  
3. [Architectural Design](#3.-architectural-design)   
* [3.1 System Block Diagram](#3.1-system-block-diagram)  
* [3.2 Datapath Design (8-bit)](#3.2-datapath-design-\(8-bit\))   
* [3.3 Controller FSM (12 States)](#3.3-controller-fsm-\(12-states\))   
* [3.4 RAM Memory Requirements](#3.4-ram-memory-requirements)  
4. [I/O Pin Assignment and Usage](#4.-i/o-pin-assignment-and-usage)  
*  [4.1 Pin Requirements](#4.1-pin-requirements)  
*  [4.2 Memory Initialization](#4.2-memory-initialization)  
5. [Area Constraints and Optimization](#5.-area-constraints-and-optimization)   
* [5.1 Area Optimization Strategies](#5.1-area-optimization-strategies)  
6. [Test Plan](#6.-test-plan)   
* [6.1 Unit Tests](#6.1-unit-tests)   
* [6.2 FSM State Testing](#6.2-fsm-state-testing)   
* [6.3 Integration Tests](#6.3-integration-tests)  
7. [Implementation Plan](#7.-implementation-plan)  
8. [Conclusion](#8.-conclusion)  
9. [Glossary of Terms](#9.-glossary-of-terms)

## **1\. Executive Summary** {#1.-executive-summary}

We propose to design and implement a [Universal Asynchronous Receiver-Transmitter (UART)](#9.-glossary-of-terms) controller in SystemC. This controller will facilitate serial communication between digital systems, providing a reliable interface with configurable parameters. Our design incorporates a 12-state Finite State Machine ([FSM](#9.-glossary-of-terms)) controller, an 8-bit datapath, and dedicated RAM storage for buffers and configuration settings.

## **2\. Project Description** {#2.-project-description}

### **2.1 Overview and Purpose** {#2.1-overview-and-purpose}

The UART controller will provide bidirectional serial communication capabilities, supporting standard UART protocols with configurable baud rates, data formats, and error detection. This controller would be valuable in various applications including embedded systems, IoT devices, and general-purpose computing where serial interfaces are required.

### **2.2 Key Features** {#2.2-key-features}

* [Full-duplex](#9.-glossary-of-terms) operation (simultaneous transmission and reception)  
* Configurable [baud rate](#9.-glossary-of-terms) generation  
* Error detection ([parity, framing, overrun)](#9.-glossary-of-terms)  
* Buffered operation with configurable [FIFO](#9.-glossary-of-terms) depths

## **3\. Architectural Design** {#3.-architectural-design}

### **3.1 System Block Diagram** {#3.1-system-block-diagram}

Our UART controller consists of three main components:

* Transmitter module  
* Receiver module  
* Control and status registers

These components interact with RAM storage and are governed by the FSM controller.

### **3.2 Datapath Design (8-bit)** {#3.2-datapath-design-(8-bit)}

The datapath for our UART controller is 8-bit wide and is the natural choice for a UART as it aligns with standard byte-oriented data transmission.

Key datapath components include:

* 8-bit transmit shift register  
* 8-bit receive shift register  
* 8-bit baud rate divisor registers  
* 8-bit status and control registers  
* 8-bit data buffers

Data flows through the following paths:

* Transmit Path: Data from RAM buffer → Transmit holding register → Transmit shift register → Serial output  
* Receive Path: Serial input → Receive shift register → Receive holding register → RAM buffer

### **3.3 Controller FSM (12 States)** {#3.3-controller-fsm-(12-states)}

The controller is implemented as a 12-state FSM that manages both transmission and reception operations:

* IDLE: System waiting for transmission request or incoming data  
* LOAD\_TX: Loading transmit shift register with data from buffer  
* TX\_START\_BIT: Transmitting start bit (logic low)  
* TX\_DATA\_BITS: Transmitting 8 data bits serially  
* TX\_PARITY\_BIT: Calculating and transmitting parity bit (if enabled)  
* TX\_STOP\_BIT: Transmitting stop bit (logic high)  
* RX\_WAIT: Waiting for start bit on receive line  
* RX\_START\_BIT: Sampling start bit and synchronizing  
* RX\_DATA\_BITS: Receiving 8 data bits serially  
* RX\_PARITY\_CHECK: Receiving and checking parity bit (if enabled)  
* RX\_STOP\_BIT: Verifying stop bit  
* ERROR\_HANDLING: Managing framing, parity, or overrun errors

State transitions are triggered by clock edges, bit counters, and input conditions. The FSM generates control signals for data movement, shift operations, and error handling.

### **3.4 RAM Memory Requirements** {#3.4-ram-memory-requirements}

Our design requires the following RAM storage:

* Transmit Buffer: 16 × 8 bits (16 bytes)  
* Receive Buffer: 16 × 8 bits (16 bytes)  
* Configuration Registers: 6 × 8 bits (6 bytes)  
  * Baud rate divisor (2 bytes): Controls communication speed by dividing the system clock  
  * Line control register (1 byte): Configures data format (bits per character, parity, stop bits)  
  * FIFO control register (1 byte): Manages buffer behavior  
  * Scratch register (2 bytes): General-purpose storage for testing or custom functions  
* Status Registers: 2 × 8 bits (2 bytes)  
  * Line status register (1 byte): Indicates transmission status and errors  
  * FIFO status register (1 byte): Shows buffer fill levels

Total RAM required: 40 bytes (40 × 8 bits)

## **4\. I/O Pin Assignment and Usage** {#4.-i/o-pin-assignment-and-usage}

### **4.1 Pin Requirements** {#4.1-pin-requirements}

Our UART controller will use the following I/O pins:

* Data Bus (Input/Output): 8 pins  
* Address Bus (Input): 6 pins (to address 40 bytes of RAM)  
* Serial Data Line (Input): 1 pin (RXD)  
* Serial Data Line (Output): 1 pin (TXD)  
* Control Signals:  
  * Chip select (Input): 1 pin  
  * Read/Write (Input): 1 pin  
  * RAM Write Enable (Input): 1 pin  
* Status Signals (Output):  
  * TX Buffer Full: 1 pin  
  * RX Buffer Empty: 1 pin  
  * Error Indicator: 1 pin

Total I/O pins: 23 pins

This is well within the 51 I/O pin limit, providing ample margin for potential future enhancements.

### **4.2 Memory Initialization** {#4.2-memory-initialization}

To program the initial memory, we will implement a memory initialization sequence that operates as follows:

1. Assert reset signal to clear all registers  
2. Use the address bus to select the memory location  
3. Place data on the data bus  
4. Assert the RAM Write Enable signal to store the data  
5. Repeat steps 2-4 for all memory locations that need initialization

This approach aligns with the recommended method of resetting memory and loading it using a memory write enable signal.

## **5\. Area Constraints and Optimization** {#5.-area-constraints-and-optimization}

### **5.1 Area Optimization Strategies** {#5.1-area-optimization-strategies}

If synthesis results exceed the area constraint, we will employ the following optimization strategies:

Memory Reduction:

* Reduce transmit and receive buffer sizes from 16 to 8 bytes each  
* Optimize configuration register usage by combining related functions

FSM Optimization:

* Apply state encoding techniques to minimize logic (one-hot, gray code)  
* Consider state reduction by combining compatible states

Datapath Optimization:

* Share resources between transmit and receive paths where possible  
* Optimize shift register implementations

I/O Pin Reduction:

* Combine status signals into a single status register accessible via the data bus  
* Multiplex address and data buses if necessary

These optimizations would be applied incrementally until the design meets the area constraint.

## **6\. Test Plan** {#6.-test-plan}

### **6.1 Unit Tests** {#6.1-unit-tests}

Transmitter Unit Tests

* Basic Transmission Test: Verify transmission of single characters  
* Continuous Transmission Test: Verify multiple consecutive transmissions  
* Parity Generation Test: Verify correct parity bit calculation  
* Baud Rate Test: Verify correct timing for various baud rates

Receiver Unit Tests

* Basic Reception Test: Verify reception of single characters  
* Continuous Reception Test: Verify multiple consecutive receptions  
* Parity Check Test: Verify parity error detection  
* Framing Error Test: Verify framing error detection  
* Overrun Error Test: Verify buffer overrun detection

Buffer Management Tests

* Buffer Full Test: Verify behavior when transmit buffer is full  
* Buffer Empty Test: Verify behavior when receive buffer is empty  
* FIFO Order Test: Verify FIFO ordering of data in buffers

### **6.2 FSM State Testing** {#6.2-fsm-state-testing}

For each of the 12 FSM states, we will develop specific tests:

IDLE State Tests:

* Verify transition to LOAD\_TX upon transmission request  
* Verify transition to RX\_WAIT upon detection of incoming data

LOAD\_TX State Tests:

* Verify data loading from buffer to shift register  
* Verify transition to TX\_START\_BIT

TX\_START\_BIT State Tests:

* Verify correct start bit timing  
* Verify transition to TX\_DATA\_BITS

TX\_DATA\_BITS State Tests:

* Verify correct bit transmission order  
* Verify correct bit timing  
* Verify transition to next state based on parity configuration

TX\_PARITY\_BIT State Tests:

* Verify correct parity calculation  
* Verify correct parity bit timing  
* Verify transition to TX\_STOP\_BIT

TX\_STOP\_BIT State Tests:

* Verify correct stop bit timing  
* Verify transition back to IDLE or LOAD\_TX based on buffer status

RX\_WAIT State Tests:

* Verify detection of start bit  
* Verify transition to RX\_START\_BIT

RX\_START\_BIT State Tests:

* Verify correct start bit sampling  
* Verify transition to RX\_DATA\_BITS

RX\_DATA\_BITS State Tests:

* Verify correct bit reception order  
* Verify correct bit sampling timing  
* Verify transition based on parity configuration

RX\_PARITY\_CHECK State Tests:

* Verify correct parity checking  
* Verify error flagging for parity errors  
* Verify transition to RX\_STOP\_BIT

RX\_STOP\_BIT State Tests:

* Verify correct stop bit sampling  
* Verify error flagging for framing errors  
* Verify transition back to RX\_WAIT or IDLE

ERROR\_HANDLING State Tests:

* Verify correct error status register updates  
* Verify transition back to operational states

### **6.3 Integration Tests** {#6.3-integration-tests}

* Loopback Test: Connect TX to RX and verify data integrity  
* Various Configuration Tests: Test different baud rates, parity settings

## **7\. Implementation Plan** {#7.-implementation-plan}

Our implementation will follow these steps:

1. Develop detailed specifications and interfaces  
2. Implement individual modules in SystemC  
3. Develop the FSM controller  
4. Integrate modules and controller  
5. Develop comprehensive test bench  
6. Execute tests and analyze results  
7. Optimize based on results and area constraints

## **8\. Conclusion** {#8.-conclusion}

The proposed UART controller meets all the specified requirements:

* Features a 12-state FSM controller  
* Implements an 8-bit datapath (exceeding the 4-bit minimum)  
* Uses RAM for buffers and configuration  
* Requires only 23 I/O pins (within the 51-pin limit)  
* Incorporates strategies for area optimization  
* Includes a comprehensive test suite

## **9\. Glossary of Terms**  {#9.-glossary-of-terms}

**UART (Universal Asynchronous Receiver-Transmitter):** A communication system that converts parallel data (multiple bits at once) from a computer into serial data (one bit at a time) for transmission over a single wire, and vice versa.

**Baud Rate:** The speed at which data is transmitted, measured in bits per second. Common baud rates include 9600, 115200, etc. 

**Full-duplex Operation:** The ability to transmit and receive data simultaneously, like a telephone conversation where both parties can speak and listen at the same time.

**FIFO (First In, First Out):** A method of organizing and manipulating data where the oldest (first) entry is processed first.

**FSM (Finite State Machine):** A mathematical model of computation that can be in exactly one of a finite number of states at any given time. A flowchart that determines what happens next based on the current situation and inputs received.

**Parity Bit:** An extra bit added to data for error-checking purposes. Ensures a specific parity is maintained. 

**Framing Error:** Occurs when the receiver doesn't detect the expected stop bit. 

**Overrun Error:** Happens when new data arrives before the previous data has been processed. 

