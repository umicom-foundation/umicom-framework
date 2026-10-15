/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/reconciliation/coverage.c
 *
 * PURPOSE:
 *   Apply Reconciliation Coverage without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/financial_platform/reconciliation/coverage.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_reconciliation_coverage(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "reconciliation.coverage",
        "Reconciliation Coverage",
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

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_financial_platform_evaluate_reconciliation_coverage_batch, umi_financial_platform_evaluate_reconciliation_coverage,
    UmiFinancialPlatformInput, UmiFinancialPlatformSnapshot)
