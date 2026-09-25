/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/liquidity/funding_requirement.c
 *
 * PURPOSE:
 *   Apply Funding Requirement without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/liquidity/funding_requirement.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_liquidity_funding_requirement(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "liquidity.funding-requirement",
        "Funding Requirement",
        UMI_OPEN_TMS_POLICY_SHORTFALL,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
