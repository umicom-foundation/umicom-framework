/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/currency/precision_validation.c
 *
 * PURPOSE:
 *   Apply Currency Precision Validation without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/currency/precision_validation.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_currency_precision_validation(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "currency.precision-validation",
        "Currency Precision Validation",
        UMI_FINANCIAL_POLICY_VALIDATION,
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
