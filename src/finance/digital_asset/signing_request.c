/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/signing_request.c
 *
 * PURPOSE:
 *   Implement a governed signing request without carrying private key material.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/signing_request.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_signing_request_init(UmiDigitalSigningRequest *value, const char *id, const char *transaction_id, const char *policy_id, uint32_t required_approvals)
{
    UmiStatus status;
    if (value == NULL || required_approvals == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->transaction_id.value, sizeof value->transaction_id.value, transaction_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->policy_id.value, sizeof value->policy_id.value, policy_id);
    if (status != UMI_STATUS_OK) return status;
    value->required_approvals = required_approvals;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_signing_request_valid(const UmiDigitalSigningRequest *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->transaction_id.value) && value->required_approvals > 0U && value->received_approvals <= value->required_approvals);
}
