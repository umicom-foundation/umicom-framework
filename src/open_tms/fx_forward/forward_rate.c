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
