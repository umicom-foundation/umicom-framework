/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/machine_block.c
 *
 * PURPOSE:
 *   Store selected machine instructions under stable block labels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/machine_block.h"
#include <string.h>
/*
 * Initialise nc machine block from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_machine_block_init(UmiNativeMachineBlock *b,uint32_t id,const char *label){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL||id==0U||label==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(b,0,sizeof(*b));b->id=id;return umi_nc_copy_text(b->label,sizeof(b->label),label);}
/* Add nc machine block only after its inputs and available capacity have been checked. */
UmiStatus umi_nc_machine_block_append(UmiNativeMachineBlock *b,const UmiNativeMachineInstruction *i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL||i==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b->instruction_count>=UMI_NC_MAX_MACHINE_INSTRUCTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;b->instructions[b->instruction_count++]=*i;return UMI_STATUS_OK;}
