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
