/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/portfolio/risk_budget.c
 *
 * PURPOSE:
 *   Apply the shared Risk Budget calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/portfolio/risk_budget.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_portfolio_risk_budget(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "portfolio.risk-budget",
        "Risk Budget",
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
