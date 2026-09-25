/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/execution/risk_limit_use.c
 *
 * PURPOSE:
 *   Apply the shared Risk Limit Utilisation calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/execution/risk_limit_use.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_execution_risk_limit_use(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "execution.risk-limit-use",
        "Risk Limit Utilisation",
        UMI_TRADING_POLICY_UTILISATION,
        0.000000,
        85.000000,
        UINT32_C(4),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
