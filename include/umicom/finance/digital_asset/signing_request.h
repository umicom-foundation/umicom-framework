/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/signing_request.h
 *
 * PURPOSE:
 *   Define a governed signing request without carrying private key material.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_SIGNING_REQUEST_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_SIGNING_REQUEST_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital signing request data shared with callers of this public contract.
 */
typedef struct UmiDigitalSigningRequest {
    UmiDigitalAssetId id;
    UmiDigitalAssetId transaction_id;
    UmiDigitalAssetId policy_id;
    uint32_t required_approvals;
    uint32_t received_approvals;
    bool closed;
} UmiDigitalSigningRequest;

/* Initialise a bounded signing request record for reusable Framework workflows. */
UmiStatus umi_digital_asset_signing_request_init(UmiDigitalSigningRequest *value, const char *id, const char *transaction_id, const char *policy_id, uint32_t required_approvals);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_signing_request_valid(const UmiDigitalSigningRequest *value);

#ifdef __cplusplus
}
#endif

#endif
