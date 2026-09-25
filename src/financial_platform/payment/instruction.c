/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/payment/instruction.c
 *
 * PURPOSE:
 *   Apply Payment Instruction without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/payment/instruction.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_payment_instruction(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "payment.instruction",
        "Payment Instruction",
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
