/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/wallet_book.h
 *
 * PURPOSE:
 *   Define a bounded book of operational and custody wallets.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_WALLET_BOOK_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_WALLET_BOOK_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"
#include "umicom/finance/digital_asset/wallet.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital asset wallet book data shared with callers of this public
 * contract.
 */
typedef struct UmiDigitalAssetWalletBook {
    UmiDigitalAssetWallet items[UMI_DIGITAL_ASSET_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiDigitalAssetWalletBook;

/* Reset a bounded catalogue before loading reference data. */
void umi_digital_asset_wallet_book_init(UmiDigitalAssetWalletBook *catalogue);

/* Add a unique item and advance the deterministic catalogue revision. */
UmiStatus umi_digital_asset_wallet_book_add(UmiDigitalAssetWalletBook *catalogue, const UmiDigitalAssetWallet *item);

/* Resolve an item by its stable Framework identifier. */
const UmiDigitalAssetWallet *umi_digital_asset_wallet_book_find(const UmiDigitalAssetWalletBook *catalogue, const char *id);

#ifdef __cplusplus
}
#endif

#endif
