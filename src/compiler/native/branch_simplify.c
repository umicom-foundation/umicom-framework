/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/branch_simplify.c
 *
 * PURPOSE:
 *   Simplify conditional branches whose condition is a compile-time IR constant.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/branch_simplify.h"
static const UmiNativeIrInstruction *find_def(const UmiNativeIrFunction *f,uint32_t id){for(size_t b=0U;b<f->block_count;b++)for(size_t i=0U;i<f->blocks[b].instruction_count;i++)if(f->blocks[b].instructions[i].result_id==id)return &f->blocks[b].instructions[i];return NULL;}
UmiStatus umi_nc_branch_simplify(UmiNativeIrFunction *f,size_t *out){if(f==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;size_t n=0U;for(size_t b=0U;b<f->block_count;b++)for(size_t i=0U;i<f->blocks[b].instruction_count;i++){UmiNativeIrInstruction *in=&f->blocks[b].instructions[i];if(in->opcode!=UMI_NC_IR_CBR||in->operand_count<3U)continue;const UmiNativeIrInstruction *cond=find_def(f,in->operands[0]);if(cond==NULL||cond->opcode!=UMI_NC_IR_CONST)continue;uint32_t target=cond->immediate!=0?in->operands[1]:in->operands[2];in->opcode=UMI_NC_IR_BR;in->operands[0]=target;in->operand_count=1U;in->side_effect=true;n++;}*out=n;return UMI_STATUS_OK;}
