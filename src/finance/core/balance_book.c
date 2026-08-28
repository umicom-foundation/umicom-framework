/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/balance_book.c
 *
 * PURPOSE:
 *   Implement bounded balance books.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/balance_book.h"

#include <string.h>
/* Reset collection. */ void umi_balance_book_init(UmiBalanceBook *c){if(c!=NULL)memset(c,0,sizeof *c);}
/* Append valid item. */ UmiStatus umi_balance_book_add(UmiBalanceBook *c,const UmiFinancialBalance *item){if(c==NULL||item==NULL||!umi_balance_is_valid(item))return UMI_STATUS_INVALID_ARGUMENT;if(c->count>=UMI_FINANCIAL_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=*item;return UMI_STATUS_OK;}
/* Return item count. */ size_t umi_balance_book_count(const UmiBalanceBook *c){return c!=NULL?c->count:0U;}
