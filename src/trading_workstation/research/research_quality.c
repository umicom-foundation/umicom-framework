/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/research/research_quality.c
 *
 * PURPOSE:
 *   Apply the shared Research Evidence Quality calculation over copied market/workspace
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
#include "umicom/trading_workstation/research/research_quality.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_research_research_quality(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "research.research-quality",
        "Research Evidence Quality",
        UMI_TRADING_POLICY_SCORE,
        65.000000,
        45.000000,
        UINT32_C(8),
        0.200,
        0.300,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_trading_professional_evaluate_research_research_quality_batch, umi_trading_professional_evaluate_research_research_quality,
    UmiTradingProfessionalInput, UmiTradingProfessionalSnapshot)
