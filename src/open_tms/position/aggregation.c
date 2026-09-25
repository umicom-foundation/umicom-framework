/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/position/aggregation.c
 *
 * PURPOSE:
 *   Apply Position Aggregation without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/position/aggregation.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_position_aggregation(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "position.aggregation",
        "Position Aggregation",
        UMI_OPEN_TMS_POLICY_CONSISTENCY,
        100.000000,
        99.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
