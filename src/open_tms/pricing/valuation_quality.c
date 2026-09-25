/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/pricing/valuation_quality.c
 *
 * PURPOSE:
 *   Apply Valuation Quality without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/pricing/valuation_quality.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_pricing_valuation_quality(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "pricing.valuation-quality",
        "Valuation Quality",
        UMI_OPEN_TMS_POLICY_SCORE,
        80.000000,
        60.000000,
        0.200,
        0.300,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
