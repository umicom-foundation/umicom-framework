/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/confirmation_policy.c
 *
 * PURPOSE:
 *   Implement network confirmation thresholds for provisional and final settlement.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/confirmation_policy.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_confirmation_policy_init(UmiDigitalConfirmationPolicy *value, const char *network_id, uint32_t required_confirmations, uint32_t final_confirmations)
{
    UmiStatus status;
    if (value == NULL || final_confirmations < required_confirmations) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    if (status != UMI_STATUS_OK) return status;
    value->required_confirmations = required_confirmations;
    value->final_confirmations = final_confirmations;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_confirmation_policy_valid(const UmiDigitalConfirmationPolicy *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->network_id.value) && value->final_confirmations >= value->required_confirmations && value->active);
}
