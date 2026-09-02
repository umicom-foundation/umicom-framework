/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/settlement_finality.c
 *
 * PURPOSE:
 *   Evaluate provisional and final settlement against explicit confirmation policy.
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

#include "umicom/finance/digital_asset/settlement_finality.h"

#include <string.h>

/* Keep confirmation-based finality deterministic and policy driven. */
UmiStatus umi_digital_asset_settlement_finality_evaluate(const UmiDigitalConfirmationPolicy *policy, uint32_t confirmations, UmiDigitalSettlementFinality *out_finality)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || out_finality == NULL || !umi_digital_asset_confirmation_policy_valid(policy)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_finality, 0, sizeof *out_finality);
    out_finality->confirmations = confirmations;
    out_finality->provisionally_settled = confirmations >= policy->required_confirmations;
    out_finality->final_settled = confirmations >= policy->final_confirmations;
    return UMI_STATUS_OK;
}
