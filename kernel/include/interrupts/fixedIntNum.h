#pragma once

#define FAULT_DIVISION_ERROR                 0
#define FAULT_TRAP_DEBUG_EXCEPTION           1
#define INT_NMI                              2
#define TRAP_BREAKPOINT                      3
#define TRAP_OVERFLOW                        4
#define FAULT_BOUND_EXCEED                   5
#define FAULT_INVALID_OPCODE                 6
#define FAULT_NO_MATH_PROCESSOR              7
#define ABORT_DOUBlE_FAULT                   8
#define FAULT_RESERVED_0                     9
#define FAULT_INVALID_TSS                   10
#define FAULT_SEG_NOT_PRESENT               11
#define FAULT_STACK_SEG_FAULT               12
#define FAULT_GENERAL_PROTECTION            13
#define FAULT_PAGE_FAULT                    14
#define FAULT_RESERVED_1                    15
#define FAULT_X87_FAULT                     16
#define FAULT_ALIGNMENT_CHECK               17
#define ABORT_MACHINE_CHECK                 18
#define FAULT_SIMD_FP_EXCEPTION             19
#define FAULT_VIRTUALIZATION_EXCEPTION      20
#define FAULT_CONTROL_PROTECTION            21
