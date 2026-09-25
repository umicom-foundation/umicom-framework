/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/execution/cancel_readiness.c
 *
 * PURPOSE:
 *   Apply the shared Cancel Readiness calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/execution/cancel_readiness.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_execution_cancel_readiness(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "execution.cancel-readiness",
        "Cancel Readiness",
        UMI_TRADING_POLICY_READINESS,
        0.000000,
        0.000000,
        UINT32_C(10),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
