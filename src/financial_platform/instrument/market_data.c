/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/instrument/market_data.c
 *
 * PURPOSE:
 *   Apply Instrument Market Data without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/instrument/market_data.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_instrument_market_data(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "instrument.market-data",
        "Instrument Market Data",
        UMI_FINANCIAL_POLICY_FRESHNESS,
        0.000000,
        60000.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
