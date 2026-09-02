/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/custody_book.h
 *
 * PURPOSE:
 *   Define a bounded registry of digital-asset custody accounts.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_CUSTODY_BOOK_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_CUSTODY_BOOK_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"
#include "umicom/finance/digital_asset/custody_account.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital custody book data shared with callers of this public contract.
 */
typedef struct UmiDigitalCustodyBook {
    UmiDigitalCustodyAccount items[UMI_DIGITAL_ASSET_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiDigitalCustodyBook;

/* Reset a bounded catalogue before loading reference data. */
void umi_digital_asset_custody_book_init(UmiDigitalCustodyBook *catalogue);

/* Add a unique item and advance the deterministic catalogue revision. */
UmiStatus umi_digital_asset_custody_book_add(UmiDigitalCustodyBook *catalogue, const UmiDigitalCustodyAccount *item);

/* Resolve an item by its stable Framework identifier. */
const UmiDigitalCustodyAccount *umi_digital_asset_custody_book_find(const UmiDigitalCustodyBook *catalogue, const char *id);

#ifdef __cplusplus
}
#endif

#endif
