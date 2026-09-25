/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/position/gross_exposure.c
 *
 * PURPOSE:
 *   Apply Gross Exposure without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/position/gross_exposure.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_position_gross_exposure(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "position.gross-exposure",
        "Gross Exposure",
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
