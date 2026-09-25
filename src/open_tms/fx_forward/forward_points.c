/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/fx_forward/forward_points.c
 *
 * PURPOSE:
 *   Apply FX Forward Points without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/fx_forward/forward_points.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_fx_forward_forward_points(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "fx_forward.forward-points",
        "FX Forward Points",
        UMI_OPEN_TMS_POLICY_FORWARD_POINTS,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
