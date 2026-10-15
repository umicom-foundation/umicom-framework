/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/fx_forward/lifecycle.c
 *
 * PURPOSE:
 *   Apply FX Forward Lifecycle without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/open_tms/fx_forward/lifecycle.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_fx_forward_lifecycle(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "fx_forward.lifecycle",
        "FX Forward Lifecycle",
        UMI_OPEN_TMS_POLICY_TRANSITION,
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
UMI_DEFINE_POLICY_BATCH(umi_open_tms_evaluate_fx_forward_lifecycle_batch, umi_open_tms_evaluate_fx_forward_lifecycle,
    UmiOpenTmsInput, UmiOpenTmsSnapshot)
