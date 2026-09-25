/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/ledger/double_entry.c
 *
 * PURPOSE:
 *   Apply Double-entry Balance without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/ledger/double_entry.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_ledger_double_entry(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "ledger.double-entry",
        "Double-entry Balance",
        UMI_FINANCIAL_POLICY_BALANCE,
        100.000000,
        99.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
