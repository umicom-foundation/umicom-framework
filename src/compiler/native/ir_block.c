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
UmiStatus umi_nc_ir_block_init(UmiNativeIrBlock *b,uint32_t id,const char *name){if(b==NULL||id==0U||name==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(b,0,sizeof(*b));b->id=id;if(umi_nc_copy_text(b->name,sizeof(b->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;}
bool umi_nc_ir_block_terminated(const UmiNativeIrBlock *b){return b!=NULL&&b->instruction_count!=0U&&umi_nc_ir_instruction_is_terminator(&b->instructions[b->instruction_count-1U]);}
UmiStatus umi_nc_ir_block_append(UmiNativeIrBlock *b,const UmiNativeIrInstruction *i){if(b==NULL||i==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_nc_ir_block_terminated(b))return UMI_STATUS_INVALID_STATE;if(b->instruction_count>=UMI_NC_MAX_IR_INSTRUCTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;b->instructions[b->instruction_count++]=*i;return UMI_STATUS_OK;}
UmiStatus umi_nc_ir_block_add_successor(UmiNativeIrBlock *b,uint32_t id){if(b==NULL||id==0U)return UMI_STATUS_INVALID_ARGUMENT;for(size_t i=0U;i<b->successor_count;i++)if(b->successors[i]==id)return UMI_STATUS_ALREADY_EXISTS;if(b->successor_count>=UMI_NC_MAX_IR_EDGES)return UMI_STATUS_CAPACITY_EXCEEDED;b->successors[b->successor_count++]=id;return UMI_STATUS_OK;}
