/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/x86_64_lowering.c
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
#include "umicom/compiler/native/x86_64_lowering.h"
const char *umi_nc_x86_64_mnemonic(UmiNativeMachineOpcode op){switch(op){case UMI_NC_MOPC_NOP:return "nop";case UMI_NC_MOPC_MOV:return "mov";case UMI_NC_MOPC_ADD:return "add";case UMI_NC_MOPC_SUB:return "sub";case UMI_NC_MOPC_MUL:return "imul";case UMI_NC_MOPC_DIV:return "idiv";case UMI_NC_MOPC_LOAD:return "mov";case UMI_NC_MOPC_STORE:return "mov";case UMI_NC_MOPC_CMP:return "cmp";case UMI_NC_MOPC_JMP:return "jmp";case UMI_NC_MOPC_JCC:return "jne";case UMI_NC_MOPC_CALL:return "call";case UMI_NC_MOPC_RET:return "ret";default:return "unknown";}}
