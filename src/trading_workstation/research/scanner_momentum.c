/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/research/scanner_momentum.c
 *
 * PURPOSE:
 *   Apply the shared Scanner Momentum calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/research/scanner_momentum.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_research_scanner_momentum(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "research.scanner-momentum",
        "Scanner Momentum",
        UMI_TRADING_POLICY_MOMENTUM,
        0.000000,
        0.000000,
        UINT32_C(1),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
