/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/cash/minimum_balance.c
 *
 * PURPOSE:
 *   Apply Minimum Cash Buffer without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/cash/minimum_balance.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_cash_minimum_balance(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "cash.minimum-balance",
        "Minimum Cash Buffer",
        UMI_OPEN_TMS_POLICY_LIMIT,
        0.000000,
        90.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
