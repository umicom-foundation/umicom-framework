/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/pricing/accrual.c
 *
 * PURPOSE:
 *   Apply Accrued Amount without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/pricing/accrual.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_pricing_accrual(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "pricing.accrual",
        "Accrued Amount",
        UMI_OPEN_TMS_POLICY_ACCRUAL,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
