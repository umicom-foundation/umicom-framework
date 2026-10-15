/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/liquidity/intraday.c
 *
 * PURPOSE:
 *   Apply Intraday Liquidity without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/open_tms/liquidity/intraday.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_liquidity_intraday(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "liquidity.intraday",
        "Intraday Liquidity",
        UMI_OPEN_TMS_POLICY_RATIO,
        100.000000,
        80.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_open_tms_evaluate_liquidity_intraday_batch, umi_open_tms_evaluate_liquidity_intraday,
    UmiOpenTmsInput, UmiOpenTmsSnapshot)
