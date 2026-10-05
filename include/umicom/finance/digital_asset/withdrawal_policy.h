/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/withdrawal_policy.h
 *
 * PURPOSE:
 *   Define daily withdrawal and approval thresholds for a custody account.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_WITHDRAWAL_POLICY_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_WITHDRAWAL_POLICY_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital withdrawal policy data shared with callers of this public
 * contract.
 */
typedef struct UmiDigitalWithdrawalPolicy {
    UmiDigitalAssetId account_id;
    int64_t daily_limit_units;
    int64_t approval_threshold_units;
    int32_t scale;
    bool address_verification_required;
    bool active;
} UmiDigitalWithdrawalPolicy;

/* Initialise a bounded withdrawal policy record for reusable Framework workflows. */
UmiStatus umi_digital_asset_withdrawal_policy_init(UmiDigitalWithdrawalPolicy *value, const char *account_id, int64_t daily_limit_units, int64_t approval_threshold_units, int32_t scale, bool address_verification_required);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_withdrawal_policy_valid(const UmiDigitalWithdrawalPolicy *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_digital_asset_withdrawal_policy_archive_encode(const UmiDigitalWithdrawalPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_digital_asset_withdrawal_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiDigitalWithdrawalPolicy *value);

#ifdef __cplusplus
}
#endif

#endif
