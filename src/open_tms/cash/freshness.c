/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/cash/freshness.c
 *
 * PURPOSE:
 *   Apply Cash Data Freshness without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/cash/freshness.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_cash_freshness(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "cash.freshness",
        "Cash Data Freshness",
        UMI_OPEN_TMS_POLICY_FRESHNESS,
        0.000000,
        60000.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
