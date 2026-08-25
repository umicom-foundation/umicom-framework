/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/copy_propagation.c
 *
 * PURPOSE:
 *   Replace uses of trivial IR copy results with their source values and erase redundant copy instructions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/copy_propagation.h"
UmiStatus umi_nc_copy_propagate(UmiNativeIrFunction *f,size_t *out_repl,size_t *out_removed){if(f==NULL||out_repl==NULL||out_removed==NULL)return UMI_STATUS_INVALID_ARGUMENT;size_t repl=0U,removed=0U;for(size_t bi=0U;bi<f->block_count;bi++){UmiNativeIrBlock *b=&f->blocks[bi];for(size_t ci=0U;ci<b->instruction_count;ci++){UmiNativeIrInstruction *copy=&b->instructions[ci];if(copy->opcode!=UMI_NC_IR_COPY||copy->result_id==0U||copy->operand_count!=1U)continue;uint32_t from=copy->operands[0],to=copy->result_id;for(size_t bj=0U;bj<f->block_count;bj++)for(size_t ii=0U;ii<f->blocks[bj].instruction_count;ii++){UmiNativeIrInstruction *in=&f->blocks[bj].instructions[ii];for(size_t oi=0U;oi<in->operand_count;oi++)if(in->operands[oi]==to){in->operands[oi]=from;repl++;}}copy->opcode=UMI_NC_IR_NOP;copy->result_id=0U;copy->operand_count=0U;removed++;}size_t w=0U;for(size_t i=0U;i<b->instruction_count;i++)if(b->instructions[i].opcode!=UMI_NC_IR_NOP){if(w!=i)b->instructions[w]=b->instructions[i];w++;}b->instruction_count=w;}*out_repl=repl;*out_removed=removed;return UMI_STATUS_OK;}
