/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/risk/liquidity.c
 *
 * PURPOSE:
 *   Apply Liquidity Risk without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/risk/liquidity.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_risk_liquidity(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "risk.liquidity",
        "Liquidity Risk",
        UMI_OPEN_TMS_POLICY_RISK_SCORE,
        80.000000,
        60.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
