/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/liquidity/stress.c
 *
 * PURPOSE:
 *   Apply Liquidity Stress Score without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/liquidity/stress.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_liquidity_stress(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "liquidity.stress",
        "Liquidity Stress Score",
        UMI_OPEN_TMS_POLICY_SCORE,
        70.000000,
        50.000000,
        0.200,
        0.300,
        0.300,
        0.200
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
