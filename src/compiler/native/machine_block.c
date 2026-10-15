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
#include "../../base/value_construction_internal.h"
#include <string.h>
/*
 * Initialise nc machine block from caller-provided values so later operations receive a
 * known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_nc_machine_block_init(UmiNativeMachineBlock *b,uint32_t id,const char *label){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL||id==0U||label==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(b,0,sizeof(*b));b->id=id;return umi_nc_copy_text(b->label,sizeof(b->label),label);}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_nc_machine_block_init_build_value(UmiNativeMachineBlock *b,uint32_t id,const char *label){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL||id==0U||label==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(b,0,sizeof(*b));b->id=id;return umi_nc_copy_text(b->label,sizeof(b->label),label);}

UMI_DEFINE_STAGED_VALUE_INIT(umi_nc_machine_block_init, UmiNativeMachineBlock, b,
    (UmiNativeMachineBlock *b, uint32_t id, const char *label),
    umi_nc_machine_block_init_build_value(candidate, id, label))

/* Add nc machine block only after its inputs and available capacity have been checked. */
UmiStatus umi_nc_machine_block_append(UmiNativeMachineBlock *b,const UmiNativeMachineInstruction *i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL||i==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b->instruction_count>=UMI_NC_MAX_MACHINE_INSTRUCTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;b->instructions[b->instruction_count++]=*i;return UMI_STATUS_OK;}
