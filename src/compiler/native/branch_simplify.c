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
/* Provide the find def operation used by this module and its client applications. */
static const UmiNativeIrInstruction *find_def(const UmiNativeIrFunction *f,uint32_t id){/* Visit each bounded item once so every record receives the same rule. */ for(size_t b=0U;b<f->block_count;b++)/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<f->blocks[b].instruction_count;i++)/* Preserve the original failure result so the caller can respond to the correct cause. */ if(f->blocks[b].instructions[i].result_id==id)return &f->blocks[b].instructions[i];return NULL;}
/*
 * Provide the nc branch simplify operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_branch_simplify(UmiNativeIrFunction *f,size_t *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;size_t n=0U;/* Visit each bounded item once so every record receives the same rule. */ for(size_t b=0U;b<f->block_count;b++)/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<f->blocks[b].instruction_count;i++){UmiNativeIrInstruction *in=&f->blocks[b].instructions[i];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(in->opcode!=UMI_NC_IR_CBR||in->operand_count<3U)continue;const UmiNativeIrInstruction *cond=find_def(f,in->operands[0]);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(cond==NULL||cond->opcode!=UMI_NC_IR_CONST)continue;uint32_t target=cond->immediate!=0?in->operands[1]:in->operands[2];in->opcode=UMI_NC_IR_BR;in->operands[0]=target;in->operand_count=1U;in->side_effect=true;n++;}*out=n;return UMI_STATUS_OK;}
