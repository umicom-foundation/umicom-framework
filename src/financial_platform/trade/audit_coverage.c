/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/trade/audit_coverage.c
 *
 * PURPOSE:
 *   Apply Trade Audit Coverage without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/trade/audit_coverage.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_trade_audit_coverage(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "trade.audit-coverage",
        "Trade Audit Coverage",
        UMI_FINANCIAL_POLICY_COVERAGE,
        100.000000,
        95.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
