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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_WALLET_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_WALLET_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital asset wallet data shared with callers of this public contract.
 */
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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_digital_asset_wallet_archive_encode(const UmiDigitalAssetWallet *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_digital_asset_wallet_archive_decode(const void *bytes, size_t byte_count,
    UmiDigitalAssetWallet *value);

#ifdef __cplusplus
}
#endif

#endif
