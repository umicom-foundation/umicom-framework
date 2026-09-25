/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/cash/sweep_readiness.c
 *
 * PURPOSE:
 *   Apply Cash Sweep Readiness without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/cash/sweep_readiness.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_cash_sweep_readiness(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "cash.sweep-readiness",
        "Cash Sweep Readiness",
        UMI_OPEN_TMS_POLICY_READINESS,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
