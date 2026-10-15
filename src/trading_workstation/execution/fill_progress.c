/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/execution/fill_progress.c
 *
 * PURPOSE:
 *   Apply the shared Fill Progress calculation over copied market/workspace
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
#include "umicom/trading_workstation/execution/fill_progress.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_execution_fill_progress(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "execution.fill-progress",
        "Fill Progress",
        UMI_TRADING_POLICY_PROGRESS,
        0.000000,
        0.000000,
        UINT32_C(2),
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
UMI_DEFINE_POLICY_BATCH(umi_trading_professional_evaluate_execution_fill_progress_batch, umi_trading_professional_evaluate_execution_fill_progress,
    UmiTradingProfessionalInput, UmiTradingProfessionalSnapshot)
