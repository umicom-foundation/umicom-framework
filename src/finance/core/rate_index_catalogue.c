/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/rate_index_catalogue.c
 *
 * PURPOSE:
 *   Implement rate-index lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/rate_index_catalogue.h"

#include <string.h>
/* Reset the bounded registry. */ void umi_rate_index_catalogue_init(UmiRateIndexCatalogue *registry){if(registry!=NULL)memset(registry,0,sizeof *registry);}
/* Find an item by identifier. */ const UmiRateIndex *umi_rate_index_catalogue_find(const UmiRateIndexCatalogue *registry,const char *id){size_t i;if(registry==NULL||id==NULL)return NULL;for(i=0U;i<registry->count;i++)if(strcmp(registry->items[i].index_id.value,id)==0)return &registry->items[i];return NULL;}
/* Add a unique item. */ UmiStatus umi_rate_index_catalogue_add(UmiRateIndexCatalogue *registry,const UmiRateIndex *item){if(registry==NULL||item==NULL||!umi_rate_index_is_valid(item))return UMI_STATUS_INVALID_ARGUMENT;if(umi_rate_index_catalogue_find(registry,item->index_id.value)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(registry->count>=UMI_FINANCIAL_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;registry->items[registry->count++]=*item;return UMI_STATUS_OK;}
