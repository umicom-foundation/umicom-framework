/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/pricing/present_value.c
 *
 * PURPOSE:
 *   Apply Present Value without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/pricing/present_value.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_pricing_present_value(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "pricing.present-value",
        "Present Value",
        UMI_OPEN_TMS_POLICY_PV,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
