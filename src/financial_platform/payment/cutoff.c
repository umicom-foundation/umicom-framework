/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/payment/cutoff.c
 *
 * PURPOSE:
 *   Apply Payment Cut-off without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/payment/cutoff.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_payment_cutoff(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "payment.cutoff",
        "Payment Cut-off",
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
