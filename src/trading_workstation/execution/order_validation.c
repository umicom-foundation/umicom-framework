/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/execution/order_validation.c
 *
 * PURPOSE:
 *   Apply the shared Order Validation calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/execution/order_validation.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_execution_order_validation(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "execution.order-validation",
        "Order Validation",
        UMI_TRADING_POLICY_ORDER_RISK,
        0.000000,
        0.000000,
        UINT32_C(1044),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
