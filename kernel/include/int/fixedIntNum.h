#pragma once

// FAULT_DIVISION_ERROR                 0
// FAULT_TRAP_DEBUG_EXCEPTION           1
// INT_NMI                              2
// TRAP_BREAKPOINT                      3
// TRAP_OVERFLOW                        4
// FAULT_BOUND_EXCEED                   5
// FAULT_INVALID_OPCODE                 6
// FAULT_NO_MATH_PROCESSOR              7
// ABORT_DOUBlE_FAULT                   8
// FAULT_RESERVED_0                     9
// FAULT_INVALID_TSS                   10
// FAULT_SEG_NOT_PRESENT               11
// FAULT_STACK_SEG_FAULT               12
// FAULT_GENERAL_PROTECTION            13
// FAULT_PAGE_FAULT                    14
// FAULT_RESERVED_1                    15
// FAULT_X87_FAULT                     16
// FAULT_ALIGNMENT_CHECK               17
// ABORT_MACHINE_CHECK                 18
// FAULT_SIMD_FP_EXCEPTION             19
// FAULT_VIRTUALIZATION_EXCEPTION      20
// FAULT_CONTROL_PROTECTION            21

#define LAPIC_INTNUM_ERROR 200
#define LAPIC_INTNUM_LINT0 201
#define LAPIC_INTNUM_TIMER 202
#define LAPIC_INTNUM_SPU 255
