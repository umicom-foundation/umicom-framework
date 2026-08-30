/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/capital_resource_book.c
 *
 * PURPOSE:
 *   Aggregate eligible capital resources and compute tier totals.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/capital_resource_book.h"

#include <string.h>
UmiStatus umi_pru_capital_resource_book_add(UmiPrudentialCapitalResourceBook *book,const UmiPrudentialCapitalResource *resource) { if(book==NULL||resource==NULL)return UMI_STATUS_INVALID_ARGUMENT; if(book->count>=UMI_PRU_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; book->items[book->count++]=*resource; return UMI_STATUS_OK; }
double umi_pru_capital_resource_book_tier_total(const UmiPrudentialCapitalResourceBook *book,UmiCapitalTierKind tier) { size_t i; double total=0.0; if(book==NULL)return 0.0; for(i=0U;i<book->count;++i) if(book->items[i].tier==tier) total+=book->items[i].eligible_amount; return total; }
