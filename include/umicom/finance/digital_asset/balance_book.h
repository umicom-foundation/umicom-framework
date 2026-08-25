/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/balance_book.h
 *
 * PURPOSE:
 *   Define a bounded book of custody-account digital-asset balances.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_BALANCE_BOOK_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_BALANCE_BOOK_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"
#include "umicom/finance/digital_asset/token_balance.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalBalanceBook {
    UmiDigitalTokenBalance items[UMI_DIGITAL_ASSET_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiDigitalBalanceBook;

/* Reset a bounded catalogue before loading reference data. */
void umi_digital_asset_balance_book_init(UmiDigitalBalanceBook *catalogue);

/* Add a unique item and advance the deterministic catalogue revision. */
UmiStatus umi_digital_asset_balance_book_add(UmiDigitalBalanceBook *catalogue, const UmiDigitalTokenBalance *item);

/* Resolve an item by its stable Framework identifier. */
const UmiDigitalTokenBalance *umi_digital_asset_balance_book_find(const UmiDigitalBalanceBook *catalogue, const char *id);

#ifdef __cplusplus
}
#endif

#endif
