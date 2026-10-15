/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/execution/order_notional.c
 *
 * PURPOSE:
 *   Apply the shared Order Notional calculation over copied market/workspace
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
#include "umicom/trading_workstation/execution/order_notional.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_execution_order_notional(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "execution.order-notional",
        "Order Notional",
        UMI_TRADING_POLICY_NOTIONAL,
        0.000000,
        0.000000,
        UINT32_C(16),
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
UMI_DEFINE_POLICY_BATCH(umi_trading_professional_evaluate_execution_order_notional_batch, umi_trading_professional_evaluate_execution_order_notional,
    UmiTradingProfessionalInput, UmiTradingProfessionalSnapshot)
