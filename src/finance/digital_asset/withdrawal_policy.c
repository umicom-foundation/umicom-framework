/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/withdrawal_policy.c
 *
 * PURPOSE:
 *   Implement daily withdrawal and approval thresholds for a custody account.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/withdrawal_policy.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_withdrawal_policy_init(UmiDigitalWithdrawalPolicy *value, const char *account_id, int64_t daily_limit_units, int64_t approval_threshold_units, int32_t scale, bool address_verification_required)
{
    UmiStatus status;
    if (value == NULL || daily_limit_units < 0 || approval_threshold_units < 0 || approval_threshold_units > daily_limit_units || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->account_id.value, sizeof value->account_id.value, account_id);
    if (status != UMI_STATUS_OK) return status;
    value->daily_limit_units = daily_limit_units;
    value->approval_threshold_units = approval_threshold_units;
    value->scale = scale;
    value->address_verification_required = address_verification_required;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_withdrawal_policy_valid(const UmiDigitalWithdrawalPolicy *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->account_id.value) && value->approval_threshold_units <= value->daily_limit_units && value->scale >= 0 && value->active);
}
