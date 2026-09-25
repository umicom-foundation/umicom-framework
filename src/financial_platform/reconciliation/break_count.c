/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/reconciliation/break_count.c
 *
 * PURPOSE:
 *   Apply Reconciliation Break Count without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/reconciliation/break_count.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_reconciliation_break_count(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "reconciliation.break-count",
        "Reconciliation Break Count",
        UMI_FINANCIAL_POLICY_INTEGRITY,
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
