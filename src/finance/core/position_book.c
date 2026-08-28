/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/position_book.c
 *
 * PURPOSE:
 *   Implement bounded position books.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/position_book.h"

#include <string.h>
/* Reset collection. */ void umi_position_book_init(UmiPositionBook *c){if(c!=NULL)memset(c,0,sizeof *c);}
/* Append valid item. */ UmiStatus umi_position_book_add(UmiPositionBook *c,const UmiPosition *item){if(c==NULL||item==NULL||!umi_position_is_valid(item))return UMI_STATUS_INVALID_ARGUMENT;if(c->count>=UMI_FINANCIAL_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=*item;return UMI_STATUS_OK;}
/* Return item count. */ size_t umi_position_book_count(const UmiPositionBook *c){return c!=NULL?c->count:0U;}
