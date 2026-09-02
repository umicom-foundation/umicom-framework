/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/signing_policy.h
 *
 * PURPOSE:
 *   Define threshold and hardware requirements for governed transaction signing.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_SIGNING_POLICY_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_SIGNING_POLICY_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital signing policy data shared with callers of this public contract.
 */
typedef struct UmiDigitalSigningPolicy {
    UmiDigitalAssetId id;
    uint32_t required_approvals;
    uint32_t available_approvers;
    bool hardware_required;
    bool active;
} UmiDigitalSigningPolicy;

/* Initialise a bounded signing policy record for reusable Framework workflows. */
UmiStatus umi_digital_asset_signing_policy_init(UmiDigitalSigningPolicy *value, const char *id, uint32_t required_approvals, uint32_t available_approvers, bool hardware_required);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_signing_policy_valid(const UmiDigitalSigningPolicy *value);

#ifdef __cplusplus
}
#endif

#endif
