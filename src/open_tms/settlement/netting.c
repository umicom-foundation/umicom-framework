/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/settlement/netting.c
 *
 * PURPOSE:
 *   Apply Settlement Netting without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/settlement/netting.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_settlement_netting(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "settlement.netting",
        "Settlement Netting",
        UMI_OPEN_TMS_POLICY_CONSISTENCY,
        100.000000,
        99.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
