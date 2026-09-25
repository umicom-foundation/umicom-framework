/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/instrument/settlement.c
 *
 * PURPOSE:
 *   Apply Instrument Settlement Readiness without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/instrument/settlement.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_instrument_settlement(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "instrument.settlement",
        "Instrument Settlement Readiness",
        UMI_FINANCIAL_POLICY_READINESS,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
