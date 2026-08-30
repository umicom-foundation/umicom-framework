/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/function_decl.c
 *
 * PURPOSE:
 *   Model native function signatures, parameter type IDs, variadic state and ABI-relevant flags.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/function_decl.h"
#include <string.h>
UmiStatus umi_nc_function_decl_init(UmiNativeFunctionDecl *d,const char *name,uint32_t ret){ if(d==NULL||name==NULL||name[0]=='\0'||ret==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(d,0,sizeof(*d));if(umi_nc_copy_text(d->name,sizeof(d->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;d->return_type_id=ret;return UMI_STATUS_OK; }
UmiStatus umi_nc_function_decl_add_parameter(UmiNativeFunctionDecl *d,uint32_t type){ if(d==NULL||type==0U)return UMI_STATUS_INVALID_ARGUMENT;if(d->parameter_count>=UMI_NC_MAX_PARAMETERS)return UMI_STATUS_CAPACITY_EXCEEDED;d->parameter_type_ids[d->parameter_count++]=type;return UMI_STATUS_OK; }
uint64_t umi_nc_function_decl_signature(const UmiNativeFunctionDecl *d){ if(d==NULL)return 0U;uint64_t h=umi_nc_hash_text(d->name)^d->return_type_id;for(size_t i=0U;i<d->parameter_count;i++)h^=(uint64_t)d->parameter_type_ids[i]+(h<<6U)+(h>>2U);h^=(uint64_t)d->variadic<<61U;return h; }
