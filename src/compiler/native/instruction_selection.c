/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/instruction_selection.c
 *
 * PURPOSE:
 *   Lower a practical IR subset into target-neutral machine instructions while reporting unsupported operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/instruction_selection.h"
#include <string.h>
/* Provide the map operation used by this module and its client applications. */
static UmiNativeMachineOpcode map(UmiNativeIrOpcode op){/* Select the behaviour associated with the requested command or state value. */ switch(op){case UMI_NC_IR_CONST:case UMI_NC_IR_COPY:return UMI_NC_MOPC_MOV;case UMI_NC_IR_ADD:return UMI_NC_MOPC_ADD;case UMI_NC_IR_SUB:return UMI_NC_MOPC_SUB;case UMI_NC_IR_MUL:return UMI_NC_MOPC_MUL;case UMI_NC_IR_DIV:return UMI_NC_MOPC_DIV;case UMI_NC_IR_LOAD:return UMI_NC_MOPC_LOAD;case UMI_NC_IR_STORE:return UMI_NC_MOPC_STORE;case UMI_NC_IR_CALL:return UMI_NC_MOPC_CALL;case UMI_NC_IR_BR:return UMI_NC_MOPC_JMP;case UMI_NC_IR_CBR:return UMI_NC_MOPC_JCC;case UMI_NC_IR_RET:return UMI_NC_MOPC_RET;default:return UMI_NC_MOPC_NOP;}}
/*
 * Provide the nc instruction select operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_instruction_select(const UmiNativeIrFunction *ir,UmiNativeMachineArch arch,UmiNativeMachineFunction *m,UmiNativeSelectionReport *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ir==NULL||m==NULL||r==NULL||arch==UMI_NC_ARCH_UNKNOWN)return UMI_STATUS_INVALID_ARGUMENT;memset(r,0,sizeof(*r));UmiStatus st=umi_nc_machine_function_init(m,ir->name,arch);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;/* Visit each bounded item once so every record receives the same rule. */ for(size_t bi=0U;bi<ir->block_count;bi++){const UmiNativeIrBlock *ib=&ir->blocks[bi];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_machine_function_add_block(m,ib->id,ib->name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;UmiNativeMachineBlock *mb=umi_nc_machine_function_block(m,ib->id);r->blocks++;/* Visit each bounded item once so every record receives the same rule. */ for(size_t ii=0U;ii<ib->instruction_count;ii++){const UmiNativeIrInstruction *in=&ib->instructions[ii];UmiNativeMachineOpcode op=map(in->opcode);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(op==UMI_NC_MOPC_NOP&&in->opcode!=UMI_NC_IR_NOP){r->unsupported++;continue;}UmiNativeMachineInstruction mi;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_machine_instruction_init(&mi,umi_nc_machine_function_next_instruction(m),op)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(in->result_id!=0U)umi_nc_machine_instruction_add_operand(&mi,umi_nc_machine_operand_register(in->result_id));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(in->opcode==UMI_NC_IR_CONST)umi_nc_machine_instruction_add_operand(&mi,umi_nc_machine_operand_immediate(in->immediate));/* Use this fallback path when the earlier condition does not apply. */ else /* Visit each bounded item once so every record receives the same rule. */ for(size_t oi=0U;oi<in->operand_count&&mi.operand_count<UMI_NC_MAX_MACHINE_OPERANDS;oi++){UmiNativeMachineOperand mo=(op==UMI_NC_MOPC_JMP||op==UMI_NC_MOPC_JCC)?umi_nc_machine_operand_block(in->operands[oi]):umi_nc_machine_operand_register(in->operands[oi]);umi_nc_machine_instruction_add_operand(&mi,mo);}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_machine_block_append(mb,&mi)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;r->lowered++;}}return r->unsupported==0U?UMI_STATUS_OK:UMI_STATUS_UNAVAILABLE;}
