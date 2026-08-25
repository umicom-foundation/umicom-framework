/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/machine_block.c
 *
 * PURPOSE:
 *   Store selected machine instructions under stable block labels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/machine_block.h"
#include <string.h>
UmiStatus umi_nc_machine_block_init(UmiNativeMachineBlock *b,uint32_t id,const char *label){if(b==NULL||id==0U||label==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(b,0,sizeof(*b));b->id=id;return umi_nc_copy_text(b->label,sizeof(b->label),label);}
UmiStatus umi_nc_machine_block_append(UmiNativeMachineBlock *b,const UmiNativeMachineInstruction *i){if(b==NULL||i==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(b->instruction_count>=UMI_NC_MAX_MACHINE_INSTRUCTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;b->instructions[b->instruction_count++]=*i;return UMI_STATUS_OK;}
