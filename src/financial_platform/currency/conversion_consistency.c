/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/currency/conversion_consistency.c
 *
 * PURPOSE:
 *   Apply FX Conversion Consistency without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/currency/conversion_consistency.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_currency_conversion_consistency(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "currency.conversion-consistency",
        "FX Conversion Consistency",
        UMI_FINANCIAL_POLICY_CONSISTENCY,
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
