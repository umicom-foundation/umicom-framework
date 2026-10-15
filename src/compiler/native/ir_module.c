/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/ir_module.c
 *
 * PURPOSE:
 *   Aggregate native IR functions under a target triple and deterministic module revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_module.h"
#include "../../base/value_construction_internal.h"
#include <string.h>
/*
 * Initialise nc ir module from caller-provided values so later operations receive a known
 * state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_nc_ir_module_init(UmiNativeIrModule *m,const char *id,const char *triple){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||id==NULL||triple==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(m,0,sizeof(*m));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(m->module_id,sizeof(m->module_id),id)!=UMI_STATUS_OK||umi_nc_copy_text(m->target_triple,sizeof(m->target_triple),triple)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_nc_ir_module_init_build_value(UmiNativeIrModule *m,const char *id,const char *triple){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||id==NULL||triple==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(m,0,sizeof(*m));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(m->module_id,sizeof(m->module_id),id)!=UMI_STATUS_OK||umi_nc_copy_text(m->target_triple,sizeof(m->target_triple),triple)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;}

UMI_DEFINE_STAGED_VALUE_INIT(umi_nc_ir_module_init, UmiNativeIrModule, m,
    (UmiNativeIrModule *m, const char *id, const char *triple),
    umi_nc_ir_module_init_build_value(candidate, id, triple))

/* Find nc ir module while leaving the underlying catalogue or model owned by this module. */
UmiNativeIrFunction *umi_nc_ir_module_find(UmiNativeIrModule *m,const char *name){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||name==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<m->function_count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(m->functions[i].name,name)==0)return &m->functions[i];return NULL;}
/*
 * Provide the nc ir module find const operation used by this module and its client
 * applications.
 */
const UmiNativeIrFunction *umi_nc_ir_module_find_const(const UmiNativeIrModule *m,const char *name){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||name==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<m->function_count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(m->functions[i].name,name)==0)return &m->functions[i];return NULL;}
/*
 * Provide the nc ir module add function operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_ir_module_add_function(UmiNativeIrModule *m,const UmiNativeIrFunction *f){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||f==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_ir_module_find(m,f->name)!=NULL)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->function_count>=UMI_NC_MAX_IR_FUNCTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;m->functions[m->function_count++]=*f;m->revision++;return UMI_STATUS_OK;}
