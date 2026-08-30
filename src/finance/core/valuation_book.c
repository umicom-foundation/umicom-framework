/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/valuation_book.c
 *
 * PURPOSE:
 *   Implement bounded valuation books.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/valuation_book.h"

#include <string.h>
/* Reset collection. */ void umi_valuation_book_init(UmiValuationBook *c){if(c!=NULL)memset(c,0,sizeof *c);}
/* Append valid item. */ UmiStatus umi_valuation_book_add(UmiValuationBook *c,const UmiValuation *item){if(c==NULL||item==NULL||!umi_valuation_is_valid(item))return UMI_STATUS_INVALID_ARGUMENT;if(c->count>=UMI_FINANCIAL_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=*item;return UMI_STATUS_OK;}
/* Return item count. */ size_t umi_valuation_book_count(const UmiValuationBook *c){return c!=NULL?c->count:0U;}
