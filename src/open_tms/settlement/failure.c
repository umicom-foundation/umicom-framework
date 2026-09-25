/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/settlement/failure.c
 *
 * PURPOSE:
 *   Apply Settlement Failure without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/settlement/failure.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_settlement_failure(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "settlement.failure",
        "Settlement Failure",
        UMI_OPEN_TMS_POLICY_BREACH,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
