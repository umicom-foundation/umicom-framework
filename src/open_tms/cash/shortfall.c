/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/cash/shortfall.c
 *
 * PURPOSE:
 *   Apply Cash Shortfall without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/cash/shortfall.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_cash_shortfall(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "cash.shortfall",
        "Cash Shortfall",
        UMI_OPEN_TMS_POLICY_SHORTFALL,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
