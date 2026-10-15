/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/chart_tape/volume_signal.c
 *
 * PURPOSE:
 *   Apply the shared Chart Volume Signal calculation over copied market/workspace
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
#include "umicom/trading_workstation/chart_tape/volume_signal.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_chart_tape_volume_signal(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "chart_tape.volume-signal",
        "Chart Volume Signal",
        UMI_TRADING_POLICY_VOLUME,
        100.000000,
        50.000000,
        UINT32_C(128),
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
UMI_DEFINE_POLICY_BATCH(umi_trading_professional_evaluate_chart_tape_volume_signal_batch, umi_trading_professional_evaluate_chart_tape_volume_signal,
    UmiTradingProfessionalInput, UmiTradingProfessionalSnapshot)
