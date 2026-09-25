/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/execution/limit_distance.c
 *
 * PURPOSE:
 *   Apply the shared Limit Distance calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/execution/limit_distance.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_execution_limit_distance(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "execution.limit-distance",
        "Limit Distance",
        UMI_TRADING_POLICY_DISTANCE,
        0.000000,
        0.000000,
        UINT32_C(32),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
