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
/*
 * Initialise nc function decl from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_function_decl_init(UmiNativeFunctionDecl *d,const char *name,uint32_t ret){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||name==NULL||name[0]=='\0'||ret==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(d,0,sizeof(*d));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(d->name,sizeof(d->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;d->return_type_id=ret;return UMI_STATUS_OK; }
/*
 * Provide the nc function decl add parameter operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_function_decl_add_parameter(UmiNativeFunctionDecl *d,uint32_t type){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||type==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d->parameter_count>=UMI_NC_MAX_PARAMETERS)return UMI_STATUS_CAPACITY_EXCEEDED;d->parameter_type_ids[d->parameter_count++]=type;return UMI_STATUS_OK; }
/*
 * Provide the nc function decl signature operation used by this module and its client
 * applications.
 */
uint64_t umi_nc_function_decl_signature(const UmiNativeFunctionDecl *d){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL)return 0U;uint64_t h=umi_nc_hash_text(d->name)^d->return_type_id;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<d->parameter_count;i++)h^=(uint64_t)d->parameter_type_ids[i]+(h<<6U)+(h>>2U);h^=(uint64_t)d->variadic<<61U;return h; }
