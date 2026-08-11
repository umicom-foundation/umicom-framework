/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/registry.c
 *
 * PURPOSE:
 *   Register independently runnable products in a bounded product catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation keeps product composition separate from reusable Framework mechanisms. It lets independent applications share the same platform without copying infrastructure.
 */
#include "umicom/product/registry.h"
#include <stdlib.h>
#include <string.h>
#define UMI_PRODUCT_REGISTRY_CAPACITY 64U
struct UmiProductRegistry{const UmiProductProfile *items[UMI_PRODUCT_REGISTRY_CAPACITY];size_t count;};
UmiStatus umi_product_registry_create(UmiProductRegistry **o){if(o==NULL)return UMI_STATUS_INVALID_ARGUMENT;*o=(UmiProductRegistry*)calloc(1U,sizeof(UmiProductRegistry));return *o==NULL?UMI_STATUS_OUT_OF_MEMORY:UMI_STATUS_OK;}
void umi_product_registry_destroy(UmiProductRegistry *r){free(r);}
UmiStatus umi_product_registry_register(UmiProductRegistry *r,const UmiProductProfile *p){size_t i;if(r==NULL||umi_product_profile_validate(p)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<r->count;++i)if(strcmp(r->items[i]->descriptor->product_id,p->descriptor->product_id)==0)return UMI_STATUS_ALREADY_EXISTS;if(r->count>=UMI_PRODUCT_REGISTRY_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;r->items[r->count++]=p;return UMI_STATUS_OK;}
const UmiProductProfile *umi_product_registry_find(const UmiProductRegistry *r,const char *id){size_t i;if(r==NULL||id==NULL)return NULL;for(i=0U;i<r->count;++i)if(strcmp(r->items[i]->descriptor->product_id,id)==0)return r->items[i];return NULL;}
size_t umi_product_registry_count(const UmiProductRegistry *r){return r==NULL?0U:r->count;}
