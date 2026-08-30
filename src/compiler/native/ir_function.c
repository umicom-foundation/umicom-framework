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
UmiStatus umi_nc_ir_function_init(UmiNativeIrFunction *f,const char *name,UmiNativeIrType ret){if(f==NULL||name==NULL||name[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(f,0,sizeof(*f));if(umi_nc_copy_text(f->name,sizeof(f->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;f->return_type=ret;f->next_block_id=1U;f->next_instruction_id=1U;f->next_value_id=1U;return UMI_STATUS_OK;}
UmiNativeIrBlock *umi_nc_ir_function_block(UmiNativeIrFunction *f,uint32_t id){if(f==NULL||id==0U)return NULL;for(size_t i=0U;i<f->block_count;i++)if(f->blocks[i].id==id)return &f->blocks[i];return NULL;}
const UmiNativeIrBlock *umi_nc_ir_function_block_const(const UmiNativeIrFunction *f,uint32_t id){if(f==NULL||id==0U)return NULL;for(size_t i=0U;i<f->block_count;i++)if(f->blocks[i].id==id)return &f->blocks[i];return NULL;}
UmiStatus umi_nc_ir_function_add_block(UmiNativeIrFunction *f,const char *name,uint32_t *out){if(f==NULL||name==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(f->block_count>=UMI_NC_MAX_IR_BLOCKS)return UMI_STATUS_CAPACITY_EXCEEDED;uint32_t id=f->next_block_id++;UmiStatus st=umi_nc_ir_block_init(&f->blocks[f->block_count],id,name);if(st!=UMI_STATUS_OK)return st;f->block_count++;if(f->entry_block_id==0U)f->entry_block_id=id;*out=id;return UMI_STATUS_OK;}
uint32_t umi_nc_ir_function_next_value(UmiNativeIrFunction *f){return f==NULL?0U:f->next_value_id++;}
uint32_t umi_nc_ir_function_next_instruction(UmiNativeIrFunction *f){return f==NULL?0U:f->next_instruction_id++;}
