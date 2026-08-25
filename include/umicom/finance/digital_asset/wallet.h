/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/wallet.h
 *
 * PURPOSE:
 *   Define a custody or operational wallet identity and its network binding.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_WALLET_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_WALLET_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalAssetWallet {
    UmiDigitalAssetId id;
    char label[UMI_DIGITAL_ASSET_NAME_CAPACITY];
    UmiDigitalAssetId network_id;
    bool custodial;
    bool enabled;
} UmiDigitalAssetWallet;

/* Initialise a bounded wallet record for reusable Framework workflows. */
UmiStatus umi_digital_asset_wallet_init(UmiDigitalAssetWallet *value, const char *id, const char *label, const char *network_id, bool custodial);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_wallet_valid(const UmiDigitalAssetWallet *value);

#ifdef __cplusplus
}
#endif

#endif
