/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/settlement/netting.c
 *
 * PURPOSE:
 *   Apply Settlement Netting without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/financial_platform/settlement/netting.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_settlement_netting(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "settlement.netting",
        "Settlement Netting",
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

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_financial_platform_evaluate_settlement_netting_batch, umi_financial_platform_evaluate_settlement_netting,
    UmiFinancialPlatformInput, UmiFinancialPlatformSnapshot)
