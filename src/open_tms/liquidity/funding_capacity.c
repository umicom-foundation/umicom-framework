/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/liquidity/funding_capacity.c
 *
 * PURPOSE:
 *   Apply Funding Capacity without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/liquidity/funding_capacity.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_liquidity_funding_capacity(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "liquidity.funding-capacity",
        "Funding Capacity",
        UMI_OPEN_TMS_POLICY_LIMIT,
        0.000000,
        85.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
