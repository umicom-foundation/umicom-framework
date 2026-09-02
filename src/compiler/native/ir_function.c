/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/ir_function.c
 *
 * PURPOSE:
 *   Own IR basic blocks and stable value/block ID allocation for one native function.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_function.h"
#include <string.h>
/*
 * Initialise nc ir function from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_ir_function_init(UmiNativeIrFunction *f,const char *name,UmiNativeIrType ret){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||name==NULL||name[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(f,0,sizeof(*f));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(f->name,sizeof(f->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;f->return_type=ret;f->next_block_id=1U;f->next_instruction_id=1U;f->next_value_id=1U;return UMI_STATUS_OK;}
/*
 * Provide the nc ir function block operation used by this module and its client
 * applications.
 */
UmiNativeIrBlock *umi_nc_ir_function_block(UmiNativeIrFunction *f,uint32_t id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||id==0U)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<f->block_count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f->blocks[i].id==id)return &f->blocks[i];return NULL;}
/*
 * Provide the nc ir function block const operation used by this module and its client
 * applications.
 */
const UmiNativeIrBlock *umi_nc_ir_function_block_const(const UmiNativeIrFunction *f,uint32_t id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||id==0U)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<f->block_count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f->blocks[i].id==id)return &f->blocks[i];return NULL;}
/*
 * Provide the nc ir function add block operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_ir_function_add_block(UmiNativeIrFunction *f,const char *name,uint32_t *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||name==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f->block_count>=UMI_NC_MAX_IR_BLOCKS)return UMI_STATUS_CAPACITY_EXCEEDED;uint32_t id=f->next_block_id++;UmiStatus st=umi_nc_ir_block_init(&f->blocks[f->block_count],id,name);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;f->block_count++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f->entry_block_id==0U)f->entry_block_id=id;*out=id;return UMI_STATUS_OK;}
/*
 * Provide the nc ir function next value operation used by this module and its client
 * applications.
 */
uint32_t umi_nc_ir_function_next_value(UmiNativeIrFunction *f){return f==NULL?0U:f->next_value_id++;}
/*
 * Provide the nc ir function next instruction operation used by this module and its client
 * applications.
 */
uint32_t umi_nc_ir_function_next_instruction(UmiNativeIrFunction *f){return f==NULL?0U:f->next_instruction_id++;}
