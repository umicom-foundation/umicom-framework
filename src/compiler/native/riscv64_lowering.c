/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/riscv64_lowering.c
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
#include "umicom/compiler/native/riscv64_lowering.h"
/*
 * Provide the nc riscv64 mnemonic operation used by this module and its client
 * applications.
 */
const char *umi_nc_riscv64_mnemonic(UmiNativeMachineOpcode op){/* Select the behaviour associated with the requested command or state value. */ switch(op){case UMI_NC_MOPC_NOP:return "nop";case UMI_NC_MOPC_MOV:return "mv";case UMI_NC_MOPC_ADD:return "add";case UMI_NC_MOPC_SUB:return "sub";case UMI_NC_MOPC_MUL:return "mul";case UMI_NC_MOPC_DIV:return "div";case UMI_NC_MOPC_LOAD:return "ld";case UMI_NC_MOPC_STORE:return "sd";case UMI_NC_MOPC_CMP:return "slt";case UMI_NC_MOPC_JMP:return "j";case UMI_NC_MOPC_JCC:return "bnez";case UMI_NC_MOPC_CALL:return "call";case UMI_NC_MOPC_RET:return "ret";default:return "unknown";}}
/*
 * Provide the nc riscv64 immediate fits i12 operation used by this module and its client
 * applications.
 */
bool umi_nc_riscv64_immediate_fits_i12(int64_t v){return v>=-2048&&v<=2047;}
