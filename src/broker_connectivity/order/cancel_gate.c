/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/order/cancel_gate.c
 *
 * PURPOSE:
 *   Apply Order Cancel Gate without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/broker_connectivity/order/cancel_gate.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_order_cancel_gate(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "order.cancel-gate",
        "Order Cancel Gate",
        UMI_BROKER_POLICY_CANCEL_GATE,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_broker_connectivity_evaluate_policy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_broker_connectivity_evaluate_order_cancel_gate_batch, umi_broker_connectivity_evaluate_order_cancel_gate,
    UmiBrokerConnectivityInput, UmiBrokerConnectivitySnapshot)
