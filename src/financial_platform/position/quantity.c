/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/position/quantity.c
 *
 * PURPOSE:
 *   Apply Position Quantity without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/position/quantity.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_position_quantity(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "position.quantity",
        "Position Quantity",
        UMI_FINANCIAL_POLICY_VALUE,
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
