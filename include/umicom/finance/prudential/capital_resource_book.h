/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/capital_resource_book.h
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_RESOURCE_BOOK_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_RESOURCE_BOOK_H

#include "umicom/finance/prudential/types.h"
#include "umicom/finance/prudential/capital_resource.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialCapitalResourceBook { UmiPrudentialCapitalResource items[UMI_PRU_MAX_ITEMS]; size_t count; } UmiPrudentialCapitalResourceBook;
/* Add one capital resource to a bounded resource book. */
UmiStatus umi_pru_capital_resource_book_add(UmiPrudentialCapitalResourceBook *book, const UmiPrudentialCapitalResource *resource);
/* Sum eligible resources for the selected capital tier. */
double umi_pru_capital_resource_book_tier_total(const UmiPrudentialCapitalResourceBook *book, UmiCapitalTierKind tier);

#ifdef __cplusplus
}
#endif

#endif
