/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/risk/stress.c
 *
 * PURPOSE:
 *   Apply Treasury Stress Score without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/risk/stress.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_risk_stress(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "risk.stress",
        "Treasury Stress Score",
        UMI_OPEN_TMS_POLICY_SCORE,
        70.000000,
        50.000000,
        0.250,
        0.250,
        0.300,
        0.200
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
