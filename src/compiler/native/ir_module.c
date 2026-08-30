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
#include <string.h>
UmiStatus umi_nc_ir_module_init(UmiNativeIrModule *m,const char *id,const char *triple){if(m==NULL||id==NULL||triple==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(m,0,sizeof(*m));if(umi_nc_copy_text(m->module_id,sizeof(m->module_id),id)!=UMI_STATUS_OK||umi_nc_copy_text(m->target_triple,sizeof(m->target_triple),triple)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;}
UmiNativeIrFunction *umi_nc_ir_module_find(UmiNativeIrModule *m,const char *name){if(m==NULL||name==NULL)return NULL;for(size_t i=0U;i<m->function_count;i++)if(strcmp(m->functions[i].name,name)==0)return &m->functions[i];return NULL;}
const UmiNativeIrFunction *umi_nc_ir_module_find_const(const UmiNativeIrModule *m,const char *name){if(m==NULL||name==NULL)return NULL;for(size_t i=0U;i<m->function_count;i++)if(strcmp(m->functions[i].name,name)==0)return &m->functions[i];return NULL;}
UmiStatus umi_nc_ir_module_add_function(UmiNativeIrModule *m,const UmiNativeIrFunction *f){if(m==NULL||f==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_nc_ir_module_find(m,f->name)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(m->function_count>=UMI_NC_MAX_IR_FUNCTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;m->functions[m->function_count++]=*f;m->revision++;return UMI_STATUS_OK;}
