/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/ir_block.c
 *
 * PURPOSE:
 *   Store ordered IR instructions and control-flow edges for a basic block.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_block.h"
#include <string.h>
/*
 * Initialise nc ir block from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_nc_ir_block_init(UmiNativeIrBlock *b,uint32_t id,const char *name){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL||id==0U||name==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(b,0,sizeof(*b));b->id=id;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(b->name,sizeof(b->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;}
/*
 * Provide the nc ir block terminated operation used by this module and its client
 * applications.
 */
bool umi_nc_ir_block_terminated(const UmiNativeIrBlock *b){return b!=NULL&&b->instruction_count!=0U&&umi_nc_ir_instruction_is_terminator(&b->instructions[b->instruction_count-1U]);}
/* Add nc ir block only after its inputs and available capacity have been checked. */
UmiStatus umi_nc_ir_block_append(UmiNativeIrBlock *b,const UmiNativeIrInstruction *i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL||i==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_ir_block_terminated(b))return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b->instruction_count>=UMI_NC_MAX_IR_INSTRUCTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;b->instructions[b->instruction_count++]=*i;return UMI_STATUS_OK;}
/*
 * Provide the nc ir block add successor operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_ir_block_add_successor(UmiNativeIrBlock *b,uint32_t id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL||id==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<b->successor_count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b->successors[i]==id)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b->successor_count>=UMI_NC_MAX_IR_EDGES)return UMI_STATUS_CAPACITY_EXCEEDED;b->successors[b->successor_count++]=id;return UMI_STATUS_OK;}
