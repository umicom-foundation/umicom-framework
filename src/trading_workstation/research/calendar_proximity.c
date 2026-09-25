/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/research/calendar_proximity.c
 *
 * PURPOSE:
 *   Apply the shared Economic Event Proximity calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/research/calendar_proximity.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_research_calendar_proximity(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "research.calendar-proximity",
        "Economic Event Proximity",
        UMI_TRADING_POLICY_EVENT,
        30.000000,
        15.000000,
        UINT32_C(8),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
