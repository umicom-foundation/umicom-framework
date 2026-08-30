/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/x86_64_lowering.h
 *
 * PURPOSE:
 *   Map target-neutral machine opcodes to x86-64 Intel-style assembly mnemonics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_X86_64_LOWERING_H
#define UMICOM_COMPILER_NATIVE_X86_64_LOWERING_H
#include "umicom/compiler/native/machine_instruction.h"
#ifdef __cplusplus
extern "C" {
#endif
const char *umi_nc_x86_64_mnemonic(UmiNativeMachineOpcode opcode);
#ifdef __cplusplus
}
#endif
#endif
