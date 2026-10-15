/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/depth/readiness.c
 *
 * PURPOSE:
 *   Apply the shared Depth Readiness calculation over copied market/workspace
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
#include "umicom/trading_workstation/depth/readiness.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_depth_readiness(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "depth.readiness",
        "Depth Readiness",
        UMI_TRADING_POLICY_READINESS,
        0.000000,
        0.000000,
        UINT32_C(81),
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
UMI_DEFINE_POLICY_BATCH(umi_trading_professional_evaluate_depth_readiness_batch, umi_trading_professional_evaluate_depth_readiness,
    UmiTradingProfessionalInput, UmiTradingProfessionalSnapshot)
