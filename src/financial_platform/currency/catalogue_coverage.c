/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/currency/catalogue_coverage.c
 *
 * PURPOSE:
 *   Apply Currency Catalogue Coverage without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/currency/catalogue_coverage.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_currency_catalogue_coverage(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "currency.catalogue-coverage",
        "Currency Catalogue Coverage",
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
