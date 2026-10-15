/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/fx_forward/forward_rate.c
 *
 * PURPOSE:
 *   Apply FX Forward Rate without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/open_tms/fx_forward/forward_rate.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_fx_forward_forward_rate(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "fx_forward.forward-rate",
        "FX Forward Rate",
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
UMI_DEFINE_POLICY_BATCH(umi_open_tms_evaluate_fx_forward_forward_rate_batch, umi_open_tms_evaluate_fx_forward_forward_rate,
    UmiOpenTmsInput, UmiOpenTmsSnapshot)
