/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/riscv64_lowering.h
 *
 * PURPOSE:
 *   Map target-neutral machine opcodes to RISC-V 64 assembly mnemonics and immediate constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_RISCV64_LOWERING_H
#define UMICOM_COMPILER_NATIVE_RISCV64_LOWERING_H
#include <stdbool.h>
#include "umicom/compiler/native/machine_instruction.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the nc riscv64 mnemonic operation used by this module and its client
 * applications.
 */
const char *umi_nc_riscv64_mnemonic(UmiNativeMachineOpcode opcode);
/**
 * Provide the nc riscv64 immediate fits i12 operation used by this module and its client
 * applications.
 */
bool umi_nc_riscv64_immediate_fits_i12(int64_t value);
#ifdef __cplusplus
}
#endif
#endif
