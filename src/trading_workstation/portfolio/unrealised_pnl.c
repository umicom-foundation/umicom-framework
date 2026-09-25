/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/portfolio/unrealised_pnl.c
 *
 * PURPOSE:
 *   Apply the shared Unrealised P&L calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/portfolio/unrealised_pnl.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_portfolio_unrealised_pnl(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "portfolio.unrealised-pnl",
        "Unrealised P&L",
        UMI_TRADING_POLICY_UNREALISED_PNL,
        0.000000,
        0.000000,
        UINT32_C(16),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
