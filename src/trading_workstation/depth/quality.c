/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/depth/quality.c
 *
 * PURPOSE:
 *   Apply the shared Depth Quality calculation over copied market/workspace
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
#include "umicom/trading_workstation/depth/quality.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_depth_quality(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "depth.quality",
        "Depth Quality",
        UMI_TRADING_POLICY_HEALTH,
        75.000000,
        50.000000,
        UINT32_C(73),
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
UMI_DEFINE_POLICY_BATCH(umi_trading_professional_evaluate_depth_quality_batch, umi_trading_professional_evaluate_depth_quality,
    UmiTradingProfessionalInput, UmiTradingProfessionalSnapshot)
