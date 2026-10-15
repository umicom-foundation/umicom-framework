/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/portfolio/realised_pnl.c
 *
 * PURPOSE:
 *   Apply the shared Realised P&L calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/trading_workstation/portfolio/realised_pnl.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_portfolio_realised_pnl(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "portfolio.realised-pnl",
        "Realised P&L",
        UMI_TRADING_POLICY_REALISED_PNL,
        0.000000,
        0.000000,
        UINT32_C(0),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_trading_professional_evaluate_portfolio_realised_pnl_batch, umi_trading_professional_evaluate_portfolio_realised_pnl,
    UmiTradingProfessionalInput, UmiTradingProfessionalSnapshot)
