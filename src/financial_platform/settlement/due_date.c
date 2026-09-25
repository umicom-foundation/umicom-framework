/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/settlement/due_date.c
 *
 * PURPOSE:
 *   Apply Settlement Due Date without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/settlement/due_date.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_settlement_due_date(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "settlement.due-date",
        "Settlement Due Date",
        UMI_FINANCIAL_POLICY_DUE,
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
