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
/*
 * Initialise nc machine function from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_machine_function_init(UmiNativeMachineFunction *f,const char *name,UmiNativeMachineArch a){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||name==NULL||a==UMI_NC_ARCH_UNKNOWN)return UMI_STATUS_INVALID_ARGUMENT;memset(f,0,sizeof(*f));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(f->name,sizeof(f->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;f->architecture=a;f->stack_alignment=16U;f->next_instruction_id=1U;return UMI_STATUS_OK;}
/*
 * Provide the nc machine function block operation used by this module and its client
 * applications.
 */
UmiNativeMachineBlock *umi_nc_machine_function_block(UmiNativeMachineFunction *f,uint32_t id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||id==0U)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<f->block_count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f->blocks[i].id==id)return &f->blocks[i];return NULL;}
/*
 * Provide the nc machine function add block operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_machine_function_add_block(UmiNativeMachineFunction *f,uint32_t id,const char *label){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||id==0U||label==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_machine_function_block(f,id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f->block_count>=UMI_NC_MAX_MACHINE_BLOCKS)return UMI_STATUS_CAPACITY_EXCEEDED;UmiStatus st=umi_nc_machine_block_init(&f->blocks[f->block_count],id,label);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)f->block_count++;return st;}
/*
 * Provide the nc machine function next instruction operation used by this module and its
 * client applications.
 */
uint32_t umi_nc_machine_function_next_instruction(UmiNativeMachineFunction *f){return f==NULL?0U:f->next_instruction_id++;}
