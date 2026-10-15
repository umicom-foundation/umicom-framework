/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/liquidity/coverage.c
 *
 * PURPOSE:
 *   Apply Liquidity Coverage without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/open_tms/liquidity/coverage.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_liquidity_coverage(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "liquidity.coverage",
        "Liquidity Coverage",
        UMI_OPEN_TMS_POLICY_RATIO,
        100.000000,
        90.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_open_tms_evaluate_liquidity_coverage_batch, umi_open_tms_evaluate_liquidity_coverage,
    UmiOpenTmsInput, UmiOpenTmsSnapshot)
