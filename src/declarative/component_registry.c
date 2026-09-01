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
UmiStatus umi_decl_component_registry_create(UmiDeclComponentRegistry **out){if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=calloc(1U,sizeof(UmiDeclComponentRegistry));return *out!=NULL?UMI_STATUS_OK:UMI_STATUS_OUT_OF_MEMORY;}
void umi_decl_component_registry_destroy(UmiDeclComponentRegistry *r){free(r);}
UmiStatus umi_decl_component_registry_register(UmiDeclComponentRegistry *r,const UmiDeclComponentDescriptor *d){UmiDeclComponentDescriptor existing;if(r==NULL||d==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_decl_component_registry_find(r,d->component_type,&existing)==UMI_STATUS_OK)return UMI_STATUS_ALREADY_EXISTS;if(r->count>=UMI_DECL_MAX_COMPONENTS)return UMI_STATUS_CAPACITY_EXCEEDED;r->items[r->count++]=*d;return UMI_STATUS_OK;}
UmiStatus umi_decl_component_registry_find(const UmiDeclComponentRegistry *r,const char *type,UmiDeclComponentDescriptor *out){size_t i;if(r==NULL||type==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<r->count;++i){if(strcmp(r->items[i].component_type,type)==0){*out=r->items[i];return UMI_STATUS_OK;}}return UMI_STATUS_NOT_FOUND;}
UmiStatus umi_decl_component_registry_at(const UmiDeclComponentRegistry *r,size_t i,UmiDeclComponentDescriptor *out){if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(i>=r->count)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
size_t umi_decl_component_registry_count(const UmiDeclComponentRegistry *r){return r!=NULL?r->count:0U;}
