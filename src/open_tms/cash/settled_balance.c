/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/cash/settled_balance.c
 *
 * PURPOSE:
 *   Apply Settled Cash Balance without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/open_tms/cash/settled_balance.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_cash_settled_balance(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "cash.settled-balance",
        "Settled Cash Balance",
        UMI_OPEN_TMS_POLICY_VALUE,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_open_tms_evaluate_cash_settled_balance_batch, umi_open_tms_evaluate_cash_settled_balance,
    UmiOpenTmsInput, UmiOpenTmsSnapshot)
