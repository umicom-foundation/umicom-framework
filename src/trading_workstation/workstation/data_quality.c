/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/workstation/data_quality.c
 *
 * PURPOSE:
 *   Apply the shared Trading Data Quality calculation over copied market/workspace
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
#include "umicom/trading_workstation/workstation/data_quality.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_workstation_data_quality(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "workstation.data-quality",
        "Trading Data Quality",
        UMI_TRADING_POLICY_HEALTH,
        80.000000,
        60.000000,
        UINT32_C(9),
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
UMI_DEFINE_POLICY_BATCH(umi_trading_professional_evaluate_workstation_data_quality_batch, umi_trading_professional_evaluate_workstation_data_quality,
    UmiTradingProfessionalInput, UmiTradingProfessionalSnapshot)
