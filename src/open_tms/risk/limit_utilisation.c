/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/risk/limit_utilisation.c
 *
 * PURPOSE:
 *   Apply Risk Limit Utilisation without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/risk/limit_utilisation.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_risk_limit_utilisation(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "risk.limit-utilisation",
        "Risk Limit Utilisation",
        UMI_OPEN_TMS_POLICY_UTILISATION,
        0.000000,
        85.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
