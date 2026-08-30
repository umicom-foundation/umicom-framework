/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/constant_folding.c
 *
 * PURPOSE:
 *   Fold integer IR operations whose operands are constants defined in the same function.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/constant_folding.h"
static const UmiNativeIrInstruction *find_def(const UmiNativeIrFunction *f,uint32_t id){for(size_t b=0U;b<f->block_count;b++)for(size_t i=0U;i<f->blocks[b].instruction_count;i++)if(f->blocks[b].instructions[i].result_id==id)return &f->blocks[b].instructions[i];return NULL;}
static bool fold(UmiNativeIrOpcode op,int64_t a,int64_t b,int64_t *out){switch(op){case UMI_NC_IR_ADD:*out=a+b;return true;case UMI_NC_IR_SUB:*out=a-b;return true;case UMI_NC_IR_MUL:*out=a*b;return true;case UMI_NC_IR_DIV:if(b==0)return false;*out=a/b;return true;case UMI_NC_IR_MOD:if(b==0)return false;*out=a%b;return true;case UMI_NC_IR_AND:*out=a&b;return true;case UMI_NC_IR_OR:*out=a|b;return true;case UMI_NC_IR_XOR:*out=a^b;return true;case UMI_NC_IR_SHL:if(b<0||b>=63)return false;*out=a<<b;return true;case UMI_NC_IR_SHR:if(b<0||b>=63)return false;*out=a>>b;return true;case UMI_NC_IR_CMP_EQ:*out=a==b?1:0;return true;case UMI_NC_IR_CMP_NE:*out=a!=b?1:0;return true;case UMI_NC_IR_CMP_LT:*out=a<b?1:0;return true;case UMI_NC_IR_CMP_LE:*out=a<=b?1:0;return true;case UMI_NC_IR_CMP_GT:*out=a>b?1:0;return true;case UMI_NC_IR_CMP_GE:*out=a>=b?1:0;return true;default:return false;}}
UmiStatus umi_nc_constant_fold_function(UmiNativeIrFunction *f,size_t *out){if(f==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;size_t n=0U;for(size_t bi=0U;bi<f->block_count;bi++)for(size_t ii=0U;ii<f->blocks[bi].instruction_count;ii++){UmiNativeIrInstruction *in=&f->blocks[bi].instructions[ii];if(in->operand_count!=2U)continue;const UmiNativeIrInstruction *a=find_def(f,in->operands[0]),*b=find_def(f,in->operands[1]);if(a==NULL||b==NULL||a->opcode!=UMI_NC_IR_CONST||b->opcode!=UMI_NC_IR_CONST)continue;int64_t value=0;if(fold(in->opcode,a->immediate,b->immediate,&value)){in->opcode=UMI_NC_IR_CONST;in->operand_count=0U;in->immediate=value;in->side_effect=false;n++;}}*out=n;return UMI_STATUS_OK;}
