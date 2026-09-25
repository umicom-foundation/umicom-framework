/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/settlement/queue.c
 *
 * PURPOSE:
 *   Apply Settlement Queue without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/settlement/queue.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_settlement_queue(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "settlement.queue",
        "Settlement Queue",
        UMI_OPEN_TMS_POLICY_COMPLETION,
        100.000000,
        90.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
