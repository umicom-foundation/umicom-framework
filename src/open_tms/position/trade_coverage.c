/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/position/trade_coverage.c
 *
 * PURPOSE:
 *   Apply Position Trade Coverage without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/position/trade_coverage.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_position_trade_coverage(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "position.trade-coverage",
        "Position Trade Coverage",
        UMI_OPEN_TMS_POLICY_COVERAGE,
        100.000000,
        95.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
