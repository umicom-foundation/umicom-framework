/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/signing_policy.c
 *
 * PURPOSE:
 *   Implement threshold and hardware requirements for governed transaction signing.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/signing_policy.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_signing_policy_init(UmiDigitalSigningPolicy *value, const char *id, uint32_t required_approvals, uint32_t available_approvers, bool hardware_required)
{
    UmiStatus status;
    if (value == NULL || required_approvals == 0U || available_approvers == 0U || required_approvals > available_approvers) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    if (status != UMI_STATUS_OK) return status;
    value->required_approvals = required_approvals;
    value->available_approvers = available_approvers;
    value->hardware_required = hardware_required;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_signing_policy_valid(const UmiDigitalSigningPolicy *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && value->required_approvals > 0U && value->required_approvals <= value->available_approvers && value->active);
}
