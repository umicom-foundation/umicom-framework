/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/ir_instruction.c
 *
 * PURPOSE:
 *   Represent typed IR instructions, operands, side effects and terminators in a compact C23 form.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_instruction.h"
#include <string.h>
UmiStatus umi_nc_ir_instruction_init(UmiNativeIrInstruction *i,uint32_t id,UmiNativeIrOpcode op,uint32_t result,UmiNativeIrType type){if(i==NULL||id==0U||op>UMI_NC_IR_COPY)return UMI_STATUS_INVALID_ARGUMENT;memset(i,0,sizeof(*i));i->id=id;i->opcode=op;i->result_id=result;i->result_type=type;i->side_effect=(op==UMI_NC_IR_STORE||op==UMI_NC_IR_CALL||op==UMI_NC_IR_RET||op==UMI_NC_IR_BR||op==UMI_NC_IR_CBR);return UMI_STATUS_OK;}
UmiStatus umi_nc_ir_instruction_add_operand(UmiNativeIrInstruction *i,uint32_t v){if(i==NULL||v==0U)return UMI_STATUS_INVALID_ARGUMENT;if(i->operand_count>=UMI_NC_MAX_IR_OPERANDS)return UMI_STATUS_CAPACITY_EXCEEDED;i->operands[i->operand_count++]=v;return UMI_STATUS_OK;}
bool umi_nc_ir_instruction_is_terminator(const UmiNativeIrInstruction *i){return i!=NULL&&(i->opcode==UMI_NC_IR_RET||i->opcode==UMI_NC_IR_BR||i->opcode==UMI_NC_IR_CBR);}
bool umi_nc_ir_instruction_produces_value(const UmiNativeIrInstruction *i){return i!=NULL&&i->result_id!=0U&&i->opcode!=UMI_NC_IR_STORE&&i->opcode!=UMI_NC_IR_BR&&i->opcode!=UMI_NC_IR_CBR&&i->opcode!=UMI_NC_IR_RET;}
