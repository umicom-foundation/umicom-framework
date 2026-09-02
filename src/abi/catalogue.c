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
/*
 * Initialise abi catalogue from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_abi_catalogue_create(UmiAbiCatalogue **out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=(UmiAbiCatalogue*)calloc(1U,sizeof(UmiAbiCatalogue));return *out==NULL?UMI_STATUS_OUT_OF_MEMORY:UMI_STATUS_OK;}
/* Release or reset state held by abi catalogue so the same storage can be reused safely. */
void umi_abi_catalogue_destroy(UmiAbiCatalogue *c){free(c);}
/* Add abi catalogue only after its inputs and available capacity have been checked. */
UmiStatus umi_abi_catalogue_register(UmiAbiCatalogue *c,const UmiAbiDescriptor *d){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||umi_abi_descriptor_validate(d)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(c->items[i]->abi_id,d->abi_id)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->count>=UMI_ABI_CATALOGUE_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=d;return UMI_STATUS_OK;}
/* Find abi catalogue while leaving the underlying catalogue or model owned by this module. */
const UmiAbiDescriptor *umi_abi_catalogue_find(const UmiAbiCatalogue *c,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||id==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(c->items[i]->abi_id,id)==0)return c->items[i];return NULL;}
/* Return the number of records represented by abi catalogue without changing their state. */
size_t umi_abi_catalogue_count(const UmiAbiCatalogue *c){return c==NULL?0U:c->count;}
