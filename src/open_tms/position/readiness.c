/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/position/readiness.c
 *
 * PURPOSE:
 *   Apply Position Readiness without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/position/readiness.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_position_readiness(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "position.readiness",
        "Position Readiness",
        UMI_OPEN_TMS_POLICY_READINESS,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
