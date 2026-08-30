/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/book_registry.c
 *
 * PURPOSE:
 *   Implement financial-book lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/book_registry.h"

#include <string.h>
/* Reset the bounded registry. */ void umi_book_registry_init(UmiBookRegistry *registry){if(registry!=NULL)memset(registry,0,sizeof *registry);}
/* Find an item by identifier. */ const UmiFinancialBook *umi_book_registry_find(const UmiBookRegistry *registry,const char *id){size_t i;if(registry==NULL||id==NULL)return NULL;for(i=0U;i<registry->count;i++)if(strcmp(registry->items[i].book_id.value,id)==0)return &registry->items[i];return NULL;}
/* Add a unique item. */ UmiStatus umi_book_registry_add(UmiBookRegistry *registry,const UmiFinancialBook *item){if(registry==NULL||item==NULL||!umi_book_is_valid(item))return UMI_STATUS_INVALID_ARGUMENT;if(umi_book_registry_find(registry,item->book_id.value)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(registry->count>=UMI_FINANCIAL_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;registry->items[registry->count++]=*item;return UMI_STATUS_OK;}
