/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/ledger/subledger.c
 *
 * PURPOSE:
 *   Apply Subledger Readiness without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/ledger/subledger.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_ledger_subledger(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "ledger.subledger",
        "Subledger Readiness",
        UMI_FINANCIAL_POLICY_READINESS,
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
