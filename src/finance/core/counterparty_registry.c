/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/counterparty_registry.c
 *
 * PURPOSE:
 *   Implement counterparty lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/counterparty_registry.h"

#include <string.h>
/* Reset the bounded registry. */ void umi_counterparty_registry_init(UmiCounterpartyRegistry *registry){if(registry!=NULL)memset(registry,0,sizeof *registry);}
/* Find an item by identifier. */ const UmiCounterparty *umi_counterparty_registry_find(const UmiCounterpartyRegistry *registry,const char *id){size_t i;if(registry==NULL||id==NULL)return NULL;for(i=0U;i<registry->count;i++)if(strcmp(registry->items[i].counterparty_id.value,id)==0)return &registry->items[i];return NULL;}
/* Add a unique item. */ UmiStatus umi_counterparty_registry_add(UmiCounterpartyRegistry *registry,const UmiCounterparty *item){if(registry==NULL||item==NULL||!umi_counterparty_is_valid(item))return UMI_STATUS_INVALID_ARGUMENT;if(umi_counterparty_registry_find(registry,item->counterparty_id.value)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(registry->count>=UMI_FINANCIAL_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;registry->items[registry->count++]=*item;return UMI_STATUS_OK;}
