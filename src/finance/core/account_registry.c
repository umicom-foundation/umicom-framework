/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/account_registry.c
 *
 * PURPOSE:
 *   Implement financial-account lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/account_registry.h"

#include <string.h>
/* Reset the bounded registry. */ void umi_account_registry_init(UmiAccountRegistry *registry){if(registry!=NULL)memset(registry,0,sizeof *registry);}
/* Find an item by identifier. */ const UmiFinancialCoreAccount *umi_account_registry_find(const UmiAccountRegistry *registry,const char *id){size_t i;if(registry==NULL||id==NULL)return NULL;for(i=0U;i<registry->count;i++)if(strcmp(registry->items[i].account_id.value,id)==0)return &registry->items[i];return NULL;}
/* Add a unique item. */ UmiStatus umi_account_registry_add(UmiAccountRegistry *registry,const UmiFinancialCoreAccount *item){if(registry==NULL||item==NULL||!umi_financial_account_is_valid(item))return UMI_STATUS_INVALID_ARGUMENT;if(umi_account_registry_find(registry,item->account_id.value)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(registry->count>=UMI_FINANCIAL_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;registry->items[registry->count++]=*item;return UMI_STATUS_OK;}
