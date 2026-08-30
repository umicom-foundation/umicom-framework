/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/token_balance.c
 *
 * PURPOSE:
 *   Implement a custody-account balance for one digital asset using integer minor units.
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

#include "umicom/finance/digital_asset/token_balance.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_token_balance_init(UmiDigitalTokenBalance *value, const char *account_id, const char *asset_id, int64_t available_units, int64_t reserved_units, int32_t scale)
{
    if (value == NULL || available_units < 0 || reserved_units < 0 || reserved_units > available_units || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    if (umi_digital_asset_copy_text(value->account_id.value, sizeof value->account_id.value, account_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_digital_asset_copy_text(value->asset_id.value, sizeof value->asset_id.value, asset_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    value->available_units = available_units;
    value->reserved_units = reserved_units;
    value->scale = scale;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_token_balance_valid(const UmiDigitalTokenBalance *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->account_id.value) && umi_digital_asset_text_valid(value->asset_id.value) && value->available_units >= value->reserved_units && value->scale >= 0);
}
