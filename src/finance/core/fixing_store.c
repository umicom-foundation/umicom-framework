/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/fixing_store.c
 *
 * PURPOSE:
 *   Implement bounded fixing stores.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/fixing_store.h"

#include <string.h>
/* Reset collection. */ void umi_fixing_store_init(UmiFixingStore *c){if(c!=NULL)memset(c,0,sizeof *c);}
/* Append valid item. */ UmiStatus umi_fixing_store_add(UmiFixingStore *c,const UmiFixing *item){if(c==NULL||item==NULL||!umi_fixing_is_valid(item))return UMI_STATUS_INVALID_ARGUMENT;if(c->count>=UMI_FINANCIAL_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=*item;return UMI_STATUS_OK;}
/* Return item count. */ size_t umi_fixing_store_count(const UmiFixingStore *c){return c!=NULL?c->count:0U;}
