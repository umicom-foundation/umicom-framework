/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/research/calendar_impact.c
 *
 * PURPOSE:
 *   Apply the shared Economic Calendar Impact calculation over copied market/workspace
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
#include "umicom/trading_workstation/research/calendar_impact.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_research_calendar_impact(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "research.calendar-impact",
        "Economic Calendar Impact",
        UMI_TRADING_POLICY_EVENT,
        70.000000,
        50.000000,
        UINT32_C(8),
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
UMI_DEFINE_POLICY_BATCH(umi_trading_professional_evaluate_research_calendar_impact_batch, umi_trading_professional_evaluate_research_calendar_impact,
    UmiTradingProfessionalInput, UmiTradingProfessionalSnapshot)
