/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/component_registry.c
 *
 * PURPOSE:
 *   Implement a bounded semantic component registry used by validators, designers and generated applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/component_registry.h"
#include <stdlib.h>
#include <string.h>
struct UmiDeclComponentRegistry{UmiDeclComponentDescriptor items[UMI_DECL_MAX_COMPONENTS];size_t count;};
/*
 * Initialise decl component registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_decl_component_registry_create(UmiDeclComponentRegistry **out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=calloc(1U,sizeof(UmiDeclComponentRegistry));return *out!=NULL?UMI_STATUS_OK:UMI_STATUS_OUT_OF_MEMORY;}
/*
 * Release or reset state held by decl component registry so the same storage can be reused
 * safely.
 */
void umi_decl_component_registry_destroy(UmiDeclComponentRegistry *r){free(r);}
/*
 * Add decl component registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_decl_component_registry_register(UmiDeclComponentRegistry *r,const UmiDeclComponentDescriptor *d){UmiDeclComponentDescriptor existing;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||d==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_decl_component_registry_find(r,d->component_type,&existing)==UMI_STATUS_OK)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->count>=UMI_DECL_MAX_COMPONENTS)return UMI_STATUS_CAPACITY_EXCEEDED;r->items[r->count++]=*d;return UMI_STATUS_OK;}
/*
 * Find decl component registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_decl_component_registry_find(const UmiDeclComponentRegistry *r,const char *type,UmiDeclComponentDescriptor *out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||type==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(r->items[i].component_type,type)==0){*out=r->items[i];return UMI_STATUS_OK;}}return UMI_STATUS_NOT_FOUND;}
/*
 * Find decl component registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_decl_component_registry_at(const UmiDeclComponentRegistry *r,size_t i,UmiDeclComponentDescriptor *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i>=r->count)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
/*
 * Return the number of records represented by decl component registry without changing
 * their state.
 */
size_t umi_decl_component_registry_count(const UmiDeclComponentRegistry *r){return r!=NULL?r->count:0U;}
