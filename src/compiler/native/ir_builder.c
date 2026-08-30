/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/ir_builder.c
 *
 * PURPOSE:
 *   Emit constants, arithmetic and return instructions into the current IR basic block.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_builder.h"
#include <string.h>
UmiStatus umi_nc_ir_builder_begin(UmiNativeIrBuilder *b,UmiNativeIrFunction *f,uint32_t block_id){if(b==NULL||f==NULL)return UMI_STATUS_INVALID_ARGUMENT;UmiNativeIrBlock *block=umi_nc_ir_function_block(f,block_id);if(block==NULL)return UMI_STATUS_NOT_FOUND;b->function=f;b->block=block;return UMI_STATUS_OK;}
UmiStatus umi_nc_ir_builder_const_i64(UmiNativeIrBuilder *b,int64_t value,uint32_t *out){if(b==NULL||b->function==NULL||b->block==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;uint32_t v=umi_nc_ir_function_next_value(b->function),iid=umi_nc_ir_function_next_instruction(b->function);UmiNativeIrInstruction i;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I64,64U,1U);if(umi_nc_ir_instruction_init(&i,iid,UMI_NC_IR_CONST,v,t)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;i.immediate=value;UmiStatus st=umi_nc_ir_block_append(b->block,&i);if(st==UMI_STATUS_OK)*out=v;return st;}
UmiStatus umi_nc_ir_builder_binary(UmiNativeIrBuilder *b,UmiNativeIrOpcode op,uint32_t l,uint32_t r,UmiNativeIrType type,uint32_t *out){if(b==NULL||b->function==NULL||b->block==NULL||out==NULL||l==0U||r==0U)return UMI_STATUS_INVALID_ARGUMENT;if(op<UMI_NC_IR_ADD||op>UMI_NC_IR_CMP_GE)return UMI_STATUS_INVALID_ARGUMENT;uint32_t v=umi_nc_ir_function_next_value(b->function),iid=umi_nc_ir_function_next_instruction(b->function);UmiNativeIrInstruction i;if(umi_nc_ir_instruction_init(&i,iid,op,v,type)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;if(umi_nc_ir_instruction_add_operand(&i,l)!=UMI_STATUS_OK||umi_nc_ir_instruction_add_operand(&i,r)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;UmiStatus st=umi_nc_ir_block_append(b->block,&i);if(st==UMI_STATUS_OK)*out=v;return st;}
UmiStatus umi_nc_ir_builder_return(UmiNativeIrBuilder *b,uint32_t value){if(b==NULL||b->function==NULL||b->block==NULL)return UMI_STATUS_INVALID_ARGUMENT;UmiNativeIrInstruction i;UmiNativeIrType v=umi_nc_ir_type_make(UMI_NC_IR_VOID,0U,1U);if(umi_nc_ir_instruction_init(&i,umi_nc_ir_function_next_instruction(b->function),UMI_NC_IR_RET,0U,v)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;if(value!=0U&&umi_nc_ir_instruction_add_operand(&i,value)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;return umi_nc_ir_block_append(b->block,&i);}
