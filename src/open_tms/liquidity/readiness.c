/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/liquidity/readiness.c
 *
 * PURPOSE:
 *   Apply Liquidity Readiness without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/liquidity/readiness.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_liquidity_readiness(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "liquidity.readiness",
        "Liquidity Readiness",
        UMI_OPEN_TMS_POLICY_HEALTH,
        90.000000,
        70.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
