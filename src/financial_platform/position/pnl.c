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
#include "../../base/policy_batch_internal.h"
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

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_financial_platform_evaluate_position_pnl_batch, umi_financial_platform_evaluate_position_pnl,
    UmiFinancialPlatformInput, UmiFinancialPlatformSnapshot)
