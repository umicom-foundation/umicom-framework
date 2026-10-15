/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/pricing/forward_rate.c
 *
 * PURPOSE:
 *   Apply Forward Pricing Rate without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/open_tms/pricing/forward_rate.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_pricing_forward_rate(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "pricing.forward-rate",
        "Forward Pricing Rate",
        UMI_OPEN_TMS_POLICY_FORWARD_RATE,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_open_tms_evaluate_pricing_forward_rate_batch, umi_open_tms_evaluate_pricing_forward_rate,
    UmiOpenTmsInput, UmiOpenTmsSnapshot)
