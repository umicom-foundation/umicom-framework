/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/reconciliation/completion.c
 *
 * PURPOSE:
 *   Apply Treasury Reconciliation Completion without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/open_tms/reconciliation/completion.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_reconciliation_completion(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "reconciliation.completion",
        "Treasury Reconciliation Completion",
        UMI_OPEN_TMS_POLICY_COMPLETION,
        100.000000,
        95.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_open_tms_evaluate_reconciliation_completion_batch, umi_open_tms_evaluate_reconciliation_completion,
    UmiOpenTmsInput, UmiOpenTmsSnapshot)
