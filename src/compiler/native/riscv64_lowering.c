/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/riscv64_lowering.c
 *
 * PURPOSE:
 *   Map target-neutral machine opcodes to RISC-V 64 assembly mnemonics and immediate constraints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/riscv64_lowering.h"
const char *umi_nc_riscv64_mnemonic(UmiNativeMachineOpcode op){switch(op){case UMI_NC_MOPC_NOP:return "nop";case UMI_NC_MOPC_MOV:return "mv";case UMI_NC_MOPC_ADD:return "add";case UMI_NC_MOPC_SUB:return "sub";case UMI_NC_MOPC_MUL:return "mul";case UMI_NC_MOPC_DIV:return "div";case UMI_NC_MOPC_LOAD:return "ld";case UMI_NC_MOPC_STORE:return "sd";case UMI_NC_MOPC_CMP:return "slt";case UMI_NC_MOPC_JMP:return "j";case UMI_NC_MOPC_JCC:return "bnez";case UMI_NC_MOPC_CALL:return "call";case UMI_NC_MOPC_RET:return "ret";default:return "unknown";}}
bool umi_nc_riscv64_immediate_fits_i12(int64_t v){return v>=-2048&&v<=2047;}
