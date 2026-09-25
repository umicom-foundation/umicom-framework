/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/pricing/spot.c
 *
 * PURPOSE:
 *   Apply Treasury Spot Price without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/pricing/spot.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_pricing_spot(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "pricing.spot",
        "Treasury Spot Price",
        UMI_OPEN_TMS_POLICY_VALUE,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
