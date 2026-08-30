/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/nomination_book.h
 *
 * PURPOSE:
 *   Define a bounded book of commodity delivery nominations.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_NOMINATION_BOOK_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_NOMINATION_BOOK_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"
#include "umicom/finance/commodity/nomination.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityNominationBook {
    UmiCommodityNomination items[UMI_COMMODITY_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiCommodityNominationBook;

/* Reset a bounded catalogue before loading reference data. */
void umi_commodity_nomination_book_init(UmiCommodityNominationBook *catalogue);

/* Add a unique item and advance the deterministic catalogue revision. */
UmiStatus umi_commodity_nomination_book_add(UmiCommodityNominationBook *catalogue, const UmiCommodityNomination *item);

/* Resolve an item by its stable Framework identifier. */
const UmiCommodityNomination *umi_commodity_nomination_book_find(const UmiCommodityNominationBook *catalogue, const char *id);

#ifdef __cplusplus
}
#endif

#endif
