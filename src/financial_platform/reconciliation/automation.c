/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/reconciliation/automation.c
 *
 * PURPOSE:
 *   Apply Reconciliation Automation without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/reconciliation/automation.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_reconciliation_automation(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "reconciliation.automation",
        "Reconciliation Automation",
        UMI_FINANCIAL_POLICY_COMPLETION,
        95.000000,
        80.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
