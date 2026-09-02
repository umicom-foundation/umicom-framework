/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/ir_verifier.c
 *
 * PURPOSE:
 *   Validate IR block termination, branch targets, instruction IDs and value-definition uniqueness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_verifier.h"
#include <string.h>
/* Provide the block exists operation used by this module and its client applications. */
static bool block_exists(const UmiNativeIrFunction *f,uint32_t id){return umi_nc_ir_function_block_const(f,id)!=NULL;}
/*
 * Provide the nc ir verify function operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_ir_verify_function(const UmiNativeIrFunction *f,UmiNativeIrVerifyReport *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||r==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(r,0,sizeof(*r));r->blocks=f->block_count;uint32_t values[UMI_NC_MAX_IR_BLOCKS*UMI_NC_MAX_IR_INSTRUCTIONS];size_t value_count=0U;/* Visit each bounded item once so every record receives the same rule. */ for(size_t bi=0U;bi<f->block_count;bi++){const UmiNativeIrBlock *b=&f->blocks[bi];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_nc_ir_block_terminated(b)){r->unterminated_blocks++;r->errors++;}/* Visit each bounded item once so every record receives the same rule. */ for(size_t ii=0U;ii<b->instruction_count;ii++){const UmiNativeIrInstruction *in=&b->instructions[ii];r->instructions++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(in->result_id!=0U){/* Visit each bounded item once so every record receives the same rule. */ for(size_t v=0U;v<value_count;v++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values[v]==in->result_id){r->duplicate_values++;r->errors++;break;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(value_count<sizeof(values)/sizeof(values[0]))values[value_count++]=in->result_id;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(in->opcode==UMI_NC_IR_BR&&in->operand_count>=1U&&!block_exists(f,in->operands[0])){r->missing_targets++;r->errors++;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(in->opcode==UMI_NC_IR_CBR&&in->operand_count>=3U&&(!block_exists(f,in->operands[1])||!block_exists(f,in->operands[2]))){r->missing_targets++;r->errors++;}}}return r->errors==0U?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;}
