/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/research/alert_trigger.c
 *
 * PURPOSE:
 *   Apply the shared Price Alert Trigger calculation over copied market/workspace
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
#include "umicom/trading_workstation/research/alert_trigger.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_research_alert_trigger(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "research.alert-trigger",
        "Price Alert Trigger",
        UMI_TRADING_POLICY_ALERT,
        0.000000,
        0.000000,
        UINT32_C(33),
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
UMI_DEFINE_POLICY_BATCH(umi_trading_professional_evaluate_research_alert_trigger_batch, umi_trading_professional_evaluate_research_alert_trigger,
    UmiTradingProfessionalInput, UmiTradingProfessionalSnapshot)
