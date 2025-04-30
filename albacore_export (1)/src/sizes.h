/**************************************************************
 * File Name: sizes.h
 * Author: Matthew Morrison
 * Date: 4/1/2025
 *
 * Contains the compiler directives the albacore processor
 **************************************************************/

#ifndef __SIZES_H__
#define __SIZES_H__

// Defining the Sizes for the datapath and memory
#define DATA_W 16
#define ADDR_W 16 
#define ALU_OP_W 4
#define NUM_REGS 16

#define RAM_SIZE 256
#define MSB_MEM 7

// Number of nanoseconds in a cycle
#define CYCLE_LENGTH 5

// FSM state constants
#define IFETCH 0
#define IFETCH2 1
#define DECODE 2
#define EX_ADD 3
#define EX_SUB 4
#define EX_AND 5
#define EX_OR 6
#define EX_NOT 7
#define EX_SHL 8
#define EX_SHR 9
#define EX_LDI 10
#define EX_LD 11
#define EX_ST 12
#define EX_BR 13
#define EX_BZ 14
#define EX_BN 15
#define EX_JAL 16
#define EX_JR 17
#define EX_QUIT 18
#define MEM_LD 19
#define MEM_LD2 20
#define MEM_ST 21
#define MEM_ST2 22
#define WB_ALU 23
#define WB_LD 24
#define WB_JAL 25
#define BR_TAKE 26
#define BR_NOT 27

#endif
