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
/*
 * Add pru capital resource book only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_pru_capital_resource_book_add(UmiPrudentialCapitalResourceBook *book,const UmiPrudentialCapitalResource *resource) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(book==NULL||resource==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(book->count>=UMI_PRU_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; book->items[book->count++]=*resource; return UMI_STATUS_OK; }
/*
 * Provide the pru capital resource book tier total operation used by this module and its
 * client applications.
 */
double umi_pru_capital_resource_book_tier_total(const UmiPrudentialCapitalResourceBook *book,UmiCapitalTierKind tier) { size_t i; double total=0.0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(book==NULL)return 0.0; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<book->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(book->items[i].tier==tier) total+=book->items[i].eligible_amount; return total; }
