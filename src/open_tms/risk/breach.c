/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/risk/breach.c
 *
 * PURPOSE:
 *   Apply Risk Limit Breach without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/risk/breach.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_risk_breach(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "risk.breach",
        "Risk Limit Breach",
        UMI_OPEN_TMS_POLICY_BREACH,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
