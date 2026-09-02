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

#ifdef __cplusplus
}
#endif

#endif
