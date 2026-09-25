/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/pricing/discount_factor.c
 *
 * PURPOSE:
 *   Apply Discount Factor without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/pricing/discount_factor.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_pricing_discount_factor(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "pricing.discount-factor",
        "Discount Factor",
        UMI_OPEN_TMS_POLICY_DISCOUNT,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
