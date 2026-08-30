/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/machine_function.c
 *
 * PURPOSE:
 *   Aggregate machine blocks, frame size and target architecture for one lowered function.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/machine_function.h"
#include <string.h>
UmiStatus umi_nc_machine_function_init(UmiNativeMachineFunction *f,const char *name,UmiNativeMachineArch a){if(f==NULL||name==NULL||a==UMI_NC_ARCH_UNKNOWN)return UMI_STATUS_INVALID_ARGUMENT;memset(f,0,sizeof(*f));if(umi_nc_copy_text(f->name,sizeof(f->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;f->architecture=a;f->stack_alignment=16U;f->next_instruction_id=1U;return UMI_STATUS_OK;}
UmiNativeMachineBlock *umi_nc_machine_function_block(UmiNativeMachineFunction *f,uint32_t id){if(f==NULL||id==0U)return NULL;for(size_t i=0U;i<f->block_count;i++)if(f->blocks[i].id==id)return &f->blocks[i];return NULL;}
UmiStatus umi_nc_machine_function_add_block(UmiNativeMachineFunction *f,uint32_t id,const char *label){if(f==NULL||id==0U||label==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_nc_machine_function_block(f,id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(f->block_count>=UMI_NC_MAX_MACHINE_BLOCKS)return UMI_STATUS_CAPACITY_EXCEEDED;UmiStatus st=umi_nc_machine_block_init(&f->blocks[f->block_count],id,label);if(st==UMI_STATUS_OK)f->block_count++;return st;}
uint32_t umi_nc_machine_function_next_instruction(UmiNativeMachineFunction *f){return f==NULL?0U:f->next_instruction_id++;}
