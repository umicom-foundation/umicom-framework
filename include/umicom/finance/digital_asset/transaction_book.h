/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/transaction_book.h
 *
 * PURPOSE:
 *   Define a bounded transaction registry for custody and settlement workflows.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_TRANSACTION_BOOK_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_TRANSACTION_BOOK_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"
#include "umicom/finance/digital_asset/transaction.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital transaction book data shared with callers of this public contract.
 */
typedef struct UmiDigitalTransactionBook {
    UmiDigitalAssetTransaction items[UMI_DIGITAL_ASSET_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiDigitalTransactionBook;

/* Reset a bounded catalogue before loading reference data. */
void umi_digital_asset_transaction_book_init(UmiDigitalTransactionBook *catalogue);

/* Add a unique item and advance the deterministic catalogue revision. */
UmiStatus umi_digital_asset_transaction_book_add(UmiDigitalTransactionBook *catalogue, const UmiDigitalAssetTransaction *item);

/* Resolve an item by its stable Framework identifier. */
const UmiDigitalAssetTransaction *umi_digital_asset_transaction_book_find(const UmiDigitalTransactionBook *catalogue, const char *id);

#ifdef __cplusplus
}
#endif

#endif
