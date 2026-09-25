/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/execution/readiness.c
 *
 * PURPOSE:
 *   Apply the shared Execution Readiness calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/execution/readiness.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_execution_readiness(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "execution.readiness",
        "Execution Readiness",
        UMI_TRADING_POLICY_HEALTH,
        80.000000,
        60.000000,
        UINT32_C(1038),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
