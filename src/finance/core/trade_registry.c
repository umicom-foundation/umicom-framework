/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/trade_registry.c
 *
 * PURPOSE:
 *   Implement trade lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/trade_registry.h"

#include <string.h>
/* Reset the bounded registry. */ void umi_trade_registry_init(UmiTradeRegistry *registry){if(registry!=NULL)memset(registry,0,sizeof *registry);}
/* Find an item by identifier. */ const UmiTradeDescriptor *umi_trade_registry_find(const UmiTradeRegistry *registry,const char *id){size_t i;if(registry==NULL||id==NULL)return NULL;for(i=0U;i<registry->count;i++)if(strcmp(registry->items[i].trade_id.value,id)==0)return &registry->items[i];return NULL;}
/* Add a unique item. */ UmiStatus umi_trade_registry_add(UmiTradeRegistry *registry,const UmiTradeDescriptor *item){if(registry==NULL||item==NULL||!umi_trade_descriptor_is_valid(item))return UMI_STATUS_INVALID_ARGUMENT;if(umi_trade_registry_find(registry,item->trade_id.value)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(registry->count>=UMI_FINANCIAL_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;registry->items[registry->count++]=*item;return UMI_STATUS_OK;}
