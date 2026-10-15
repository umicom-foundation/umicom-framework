/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/pricing/market_data.c
 *
 * PURPOSE:
 *   Apply Pricing Market Data without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/open_tms/pricing/market_data.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_pricing_market_data(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "pricing.market-data",
        "Pricing Market Data",
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

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_open_tms_evaluate_pricing_market_data_batch, umi_open_tms_evaluate_pricing_market_data,
    UmiOpenTmsInput, UmiOpenTmsSnapshot)
