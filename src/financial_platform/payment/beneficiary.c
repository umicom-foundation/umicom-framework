/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/payment/beneficiary.c
 *
 * PURPOSE:
 *   Apply Payment Beneficiary without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/payment/beneficiary.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_payment_beneficiary(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "payment.beneficiary",
        "Payment Beneficiary",
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
