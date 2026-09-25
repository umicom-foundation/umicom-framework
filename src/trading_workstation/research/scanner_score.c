/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/research/scanner_score.c
 *
 * PURPOSE:
 *   Apply the shared Market Scanner Score calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/research/scanner_score.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_research_scanner_score(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "research.scanner-score",
        "Market Scanner Score",
        UMI_TRADING_POLICY_SCORE,
        65.000000,
        50.000000,
        UINT32_C(1),
        0.250,
        0.250,
        0.300,
        0.200
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
