#pragma once

#define FAULT_DIVISION_ERROR              0x00
#define FAULT_TRAP_DEBUG_EXCEPTION        0x01
#define INT_NMI                           0x02
#define TRAP_BREAKPOINT                   0x03
#define TRAP_OVERFLOW                     0x04
#define FAULT_BOUND_EXCEED                0x05
#define FAULT_INVALID_OPCODE              0x06
#define FAULT_NO_MATH_PROCESSOR           0x07
#define ABORT_DOUBlE_FAULT                0x08
#define FAULT_RESERVED_0                  0x09
#define FAULT_INVALID_TSS                 0x0A
#define FAULT_SEG_NOT_PRESENT             0x0B
#define FAULT_STACK_SEG_FAULT             0x0C
#define FAULT_GENERAL_PROTECTION          0x0D
#define FAULT_PAGE_FAULT                  0x0E
#define FAULT_RESERVED_1                  0x0F
#define FAULT_X87_FAULT                   0x10
#define FAULT_ALIGNMENT_CHECK             0x11
#define ABORT_MACHINE_CHECK               0x12
#define FAULT_SIMD_FP_EXCEPTION           0x13
#define FAULT_VIRTUALIZATION_EXCEPTION    0x14
#define FAULT_CONTROL_PROTECTION          0x15
