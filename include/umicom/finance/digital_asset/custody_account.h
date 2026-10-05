/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/custody_account.h
 *
 * PURPOSE:
 *   Define a client or house digital-asset custody account bound to a wallet.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_CUSTODY_ACCOUNT_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_CUSTODY_ACCOUNT_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital custody account data shared with callers of this public contract.
 */
typedef struct UmiDigitalCustodyAccount {
    UmiDigitalAssetId id;
    UmiFinancialId owner_party_id;
    UmiDigitalAssetId wallet_id;
    bool segregated;
    bool active;
} UmiDigitalCustodyAccount;

/* Initialise a bounded custody account record for reusable Framework workflows. */
UmiStatus umi_digital_asset_custody_account_init(UmiDigitalCustodyAccount *value, const char *id, const UmiFinancialId *owner_party_id, const char *wallet_id, bool segregated);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_custody_account_valid(const UmiDigitalCustodyAccount *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_digital_asset_custody_account_archive_encode(const UmiDigitalCustodyAccount *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_digital_asset_custody_account_archive_decode(const void *bytes, size_t byte_count,
    UmiDigitalCustodyAccount *value);

#ifdef __cplusplus
}
#endif

#endif
