/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/confirmation_policy.h
 *
 * PURPOSE:
 *   Define network confirmation thresholds for provisional and final settlement.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_CONFIRMATION_POLICY_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_CONFIRMATION_POLICY_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalConfirmationPolicy {
    UmiDigitalAssetId network_id;
    uint32_t required_confirmations;
    uint32_t final_confirmations;
    bool active;
} UmiDigitalConfirmationPolicy;

/* Initialise a bounded confirmation policy record for reusable Framework workflows. */
UmiStatus umi_digital_asset_confirmation_policy_init(UmiDigitalConfirmationPolicy *value, const char *network_id, uint32_t required_confirmations, uint32_t final_confirmations);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_confirmation_policy_valid(const UmiDigitalConfirmationPolicy *value);

#ifdef __cplusplus
}
#endif

#endif
