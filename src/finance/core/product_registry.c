/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/product_registry.c
 *
 * PURPOSE:
 *   Implement product lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/product_registry.h"

#include <string.h>
/* Reset the bounded registry. */ void umi_product_registry_init(UmiProductRegistry *registry){if(registry!=NULL)memset(registry,0,sizeof *registry);}
/* Find an item by identifier. */ const UmiProductDescriptor *umi_product_registry_find(const UmiProductRegistry *registry,const char *id){size_t i;if(registry==NULL||id==NULL)return NULL;for(i=0U;i<registry->count;i++)if(strcmp(registry->items[i].product_id.value,id)==0)return &registry->items[i];return NULL;}
/* Add a unique item. */ UmiStatus umi_product_registry_add(UmiProductRegistry *registry,const UmiProductDescriptor *item){if(registry==NULL||item==NULL||!umi_product_descriptor_is_valid(item))return UMI_STATUS_INVALID_ARGUMENT;if(umi_product_registry_find(registry,item->product_id.value)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(registry->count>=UMI_FINANCIAL_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;registry->items[registry->count++]=*item;return UMI_STATUS_OK;}
