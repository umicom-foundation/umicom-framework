/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/research/alert_density.c
 *
 * PURPOSE:
 *   Apply the shared Alert Density calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/research/alert_density.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_research_alert_density(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "research.alert-density",
        "Alert Density",
        UMI_TRADING_POLICY_RATIO,
        0.000000,
        50.000000,
        UINT32_C(1),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
