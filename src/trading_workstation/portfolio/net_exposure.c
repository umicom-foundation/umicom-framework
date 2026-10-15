/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/portfolio/net_exposure.c
 *
 * PURPOSE:
 *   Apply the shared Net Exposure calculation over copied market/workspace
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
#include "umicom/trading_workstation/portfolio/net_exposure.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_portfolio_net_exposure(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "portfolio.net-exposure",
        "Net Exposure",
        UMI_TRADING_POLICY_EXPOSURE,
        0.000000,
        0.000000,
        UINT32_C(4),
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
UMI_DEFINE_POLICY_BATCH(umi_trading_professional_evaluate_portfolio_net_exposure_batch, umi_trading_professional_evaluate_portfolio_net_exposure,
    UmiTradingProfessionalInput, UmiTradingProfessionalSnapshot)
