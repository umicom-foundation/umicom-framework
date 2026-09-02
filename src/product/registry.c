/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/registry.c
 *
 * PURPOSE:
 *   Register independently runnable products in a bounded product catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation keeps product composition separate from reusable Framework mechanisms. It lets independent applications share the same platform without copying infrastructure.
 */
#include "umicom/product/registry.h"
#include <stdlib.h>
#include <string.h>
#define UMI_PRODUCT_REGISTRY_CAPACITY 64U
struct UmiProductRegistry{const UmiProductProfile *items[UMI_PRODUCT_REGISTRY_CAPACITY];size_t count;};
/*
 * Initialise product registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_product_registry_create(UmiProductRegistry **o){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(o==NULL)return UMI_STATUS_INVALID_ARGUMENT;*o=(UmiProductRegistry*)calloc(1U,sizeof(UmiProductRegistry));return *o==NULL?UMI_STATUS_OUT_OF_MEMORY:UMI_STATUS_OK;}
/*
 * Release or reset state held by product registry so the same storage can be reused
 * safely.
 */
void umi_product_registry_destroy(UmiProductRegistry *r){free(r);}
/* Add product registry only after its inputs and available capacity have been checked. */
UmiStatus umi_product_registry_register(UmiProductRegistry *r,const UmiProductProfile *p){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||umi_product_profile_validate(p)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(r->items[i]->descriptor->product_id,p->descriptor->product_id)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->count>=UMI_PRODUCT_REGISTRY_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;r->items[r->count++]=p;return UMI_STATUS_OK;}
/*
 * Find product registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiProductProfile *umi_product_registry_find(const UmiProductRegistry *r,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(r->items[i]->descriptor->product_id,id)==0)return r->items[i];return NULL;}
/*
 * Return the number of records represented by product registry without changing their
 * state.
 */
size_t umi_product_registry_count(const UmiProductRegistry *r){return r==NULL?0U:r->count;}
