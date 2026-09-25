/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/cash/surplus.c
 *
 * PURPOSE:
 *   Apply Cash Surplus without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/cash/surplus.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_cash_surplus(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "cash.surplus",
        "Cash Surplus",
        UMI_OPEN_TMS_POLICY_SURPLUS,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
