/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/position/net_exposure.c
 *
 * PURPOSE:
 *   Apply Net Exposure without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/position/net_exposure.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_position_net_exposure(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "position.net-exposure",
        "Net Exposure",
        UMI_FINANCIAL_POLICY_EXPOSURE,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
