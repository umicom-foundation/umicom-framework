/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/catalogue.c
 *
 * PURPOSE:
 *   Store registered ABI descriptors in a bounded deterministic catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source file implements one small part of ABI validation. Keeping checks separate makes release failures easier to understand and test.
 */
#include "umicom/abi/catalogue.h"
#include <stdlib.h>
#include <string.h>
#define UMI_ABI_CATALOGUE_CAPACITY 64U
struct UmiAbiCatalogue{const UmiAbiDescriptor *items[UMI_ABI_CATALOGUE_CAPACITY];size_t count;};
UmiStatus umi_abi_catalogue_create(UmiAbiCatalogue **out){if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=(UmiAbiCatalogue*)calloc(1U,sizeof(UmiAbiCatalogue));return *out==NULL?UMI_STATUS_OUT_OF_MEMORY:UMI_STATUS_OK;}
void umi_abi_catalogue_destroy(UmiAbiCatalogue *c){free(c);}
UmiStatus umi_abi_catalogue_register(UmiAbiCatalogue *c,const UmiAbiDescriptor *d){size_t i;if(c==NULL||umi_abi_descriptor_validate(d)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<c->count;++i)if(strcmp(c->items[i]->abi_id,d->abi_id)==0)return UMI_STATUS_ALREADY_EXISTS;if(c->count>=UMI_ABI_CATALOGUE_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=d;return UMI_STATUS_OK;}
const UmiAbiDescriptor *umi_abi_catalogue_find(const UmiAbiCatalogue *c,const char *id){size_t i;if(c==NULL||id==NULL)return NULL;for(i=0U;i<c->count;++i)if(strcmp(c->items[i]->abi_id,id)==0)return c->items[i];return NULL;}
size_t umi_abi_catalogue_count(const UmiAbiCatalogue *c){return c==NULL?0U:c->count;}
