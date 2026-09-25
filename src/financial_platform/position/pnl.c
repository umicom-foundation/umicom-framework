/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/position/pnl.c
 *
 * PURPOSE:
 *   Apply Position P&L without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/position/pnl.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_position_pnl(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "position.pnl",
        "Position P&L",
        UMI_FINANCIAL_POLICY_PNL,
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
