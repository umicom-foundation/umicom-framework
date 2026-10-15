/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/workstation/professional_readiness.c
 *
 * PURPOSE:
 *   Apply the shared Professional Workstation Readiness calculation over copied market/workspace
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
#include "umicom/trading_workstation/workstation/professional_readiness.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_workstation_professional_readiness(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "workstation.professional-readiness",
        "Professional Workstation Readiness",
        UMI_TRADING_POLICY_HEALTH,
        85.000000,
        65.000000,
        UINT32_C(1549),
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
UMI_DEFINE_POLICY_BATCH(umi_trading_professional_evaluate_workstation_professional_readiness_batch, umi_trading_professional_evaluate_workstation_professional_readiness,
    UmiTradingProfessionalInput, UmiTradingProfessionalSnapshot)
