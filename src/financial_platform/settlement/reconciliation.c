/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/settlement/reconciliation.c
 *
 * PURPOSE:
 *   Apply Settlement Reconciliation without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/settlement/reconciliation.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_settlement_reconciliation(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "settlement.reconciliation",
        "Settlement Reconciliation",
        UMI_FINANCIAL_POLICY_MATCH,
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
