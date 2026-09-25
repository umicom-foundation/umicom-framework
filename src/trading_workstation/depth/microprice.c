/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/depth/microprice.c
 *
 * PURPOSE:
 *   Apply the shared Depth Microprice calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/depth/microprice.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_depth_microprice(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "depth.microprice",
        "Depth Microprice",
        UMI_TRADING_POLICY_MICROPRICE,
        0.000000,
        0.000000,
        UINT32_C(96),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
