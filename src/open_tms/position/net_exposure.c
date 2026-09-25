/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/position/net_exposure.c
 *
 * PURPOSE:
 *   Apply Net Treasury Exposure without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/position/net_exposure.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_position_net_exposure(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "position.net-exposure",
        "Net Treasury Exposure",
        UMI_OPEN_TMS_POLICY_EXPOSURE,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
