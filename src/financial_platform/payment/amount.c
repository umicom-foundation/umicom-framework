/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/payment/amount.c
 *
 * PURPOSE:
 *   Apply Payment Amount without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/payment/amount.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_payment_amount(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "payment.amount",
        "Payment Amount",
        UMI_FINANCIAL_POLICY_AMOUNT,
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
