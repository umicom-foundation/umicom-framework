/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/machine_instruction.c
 *
 * PURPOSE:
 *   Represent selected target-neutral machine operations before textual assembly or object encoding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/machine_instruction.h"
#include <string.h>
/*
 * Initialise nc machine instruction from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_nc_machine_instruction_init(UmiNativeMachineInstruction *i,uint32_t id,UmiNativeMachineOpcode op){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==NULL||id==0U||op>UMI_NC_MOPC_RET)return UMI_STATUS_INVALID_ARGUMENT;memset(i,0,sizeof(*i));i->id=id;i->opcode=op;i->side_effect=op==UMI_NC_MOPC_STORE||op==UMI_NC_MOPC_CALL||op==UMI_NC_MOPC_JMP||op==UMI_NC_MOPC_JCC||op==UMI_NC_MOPC_RET;return UMI_STATUS_OK;}
/*
 * Provide the nc machine instruction add operand operation used by this module and its
 * client applications.
 */
UmiStatus umi_nc_machine_instruction_add_operand(UmiNativeMachineInstruction *i,UmiNativeMachineOperand o){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==NULL||o.kind==UMI_NC_MOP_NONE)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i->operand_count>=UMI_NC_MAX_MACHINE_OPERANDS)return UMI_STATUS_CAPACITY_EXCEEDED;i->operands[i->operand_count++]=o;return UMI_STATUS_OK;}
/*
 * Provide the nc machine opcode name operation used by this module and its client
 * applications.
 */
const char *umi_nc_machine_opcode_name(UmiNativeMachineOpcode op){/* Select the behaviour associated with the requested command or state value. */ switch(op){case UMI_NC_MOPC_NOP:return "nop";case UMI_NC_MOPC_MOV:return "mov";case UMI_NC_MOPC_ADD:return "add";case UMI_NC_MOPC_SUB:return "sub";case UMI_NC_MOPC_MUL:return "mul";case UMI_NC_MOPC_DIV:return "div";case UMI_NC_MOPC_LOAD:return "load";case UMI_NC_MOPC_STORE:return "store";case UMI_NC_MOPC_CMP:return "cmp";case UMI_NC_MOPC_JMP:return "jmp";case UMI_NC_MOPC_JCC:return "jcc";case UMI_NC_MOPC_CALL:return "call";case UMI_NC_MOPC_RET:return "ret";default:return "unknown";}}
