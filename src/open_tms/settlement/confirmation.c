/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/settlement/confirmation.c
 *
 * PURPOSE:
 *   Apply Confirmation Matching without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/settlement/confirmation.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_settlement_confirmation(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "settlement.confirmation",
        "Confirmation Matching",
        UMI_OPEN_TMS_POLICY_MATCH,
        100.000000,
        95.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
