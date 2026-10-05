/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/token_balance.h
 *
 * PURPOSE:
 *   Define a custody-account balance for one digital asset using integer minor units.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_TOKEN_BALANCE_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_TOKEN_BALANCE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital token balance data shared with callers of this public contract.
 */
typedef struct UmiDigitalTokenBalance {
    UmiDigitalAssetId account_id;
    UmiDigitalAssetId asset_id;
    int64_t available_units;
    int64_t reserved_units;
    int32_t scale;
} UmiDigitalTokenBalance;

/* Initialise a bounded token balance record for reusable Framework workflows. */
UmiStatus umi_digital_asset_token_balance_init(UmiDigitalTokenBalance *value, const char *account_id, const char *asset_id, int64_t available_units, int64_t reserved_units, int32_t scale);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_token_balance_valid(const UmiDigitalTokenBalance *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_digital_asset_token_balance_archive_encode(const UmiDigitalTokenBalance *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_digital_asset_token_balance_archive_decode(const void *bytes, size_t byte_count,
    UmiDigitalTokenBalance *value);

#ifdef __cplusplus
}
#endif

#endif
