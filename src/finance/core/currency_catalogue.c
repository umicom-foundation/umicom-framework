/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/currency_catalogue.c
 *
 * PURPOSE:
 *   Implement a bounded currency catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/currency_catalogue.h"

#include <string.h>
/* Reset the bounded registry. */ void umi_currency_catalogue_init(UmiCurrencyCatalogue *registry){if(registry!=NULL)memset(registry,0,sizeof *registry);}
/* Find an item by identifier. */ const UmiCurrencyDescriptor *umi_currency_catalogue_find(const UmiCurrencyCatalogue *registry,const char *id){size_t i;if(registry==NULL||id==NULL)return NULL;for(i=0U;i<registry->count;i++)if(strcmp(registry->items[i].currency.code,id)==0)return &registry->items[i];return NULL;}
/* Add a unique item. */ UmiStatus umi_currency_catalogue_add(UmiCurrencyCatalogue *registry,const UmiCurrencyDescriptor *item){if(registry==NULL||item==NULL||!umi_currency_descriptor_is_valid(item))return UMI_STATUS_INVALID_ARGUMENT;if(umi_currency_catalogue_find(registry,item->currency.code)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(registry->count>=UMI_FINANCIAL_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;registry->items[registry->count++]=*item;return UMI_STATUS_OK;}
