/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/ledger/suspense.c
 *
 * PURPOSE:
 *   Apply Suspense Utilisation without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/ledger/suspense.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_ledger_suspense(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "ledger.suspense",
        "Suspense Utilisation",
        UMI_FINANCIAL_POLICY_UTILISATION,
        0.000000,
        5.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
