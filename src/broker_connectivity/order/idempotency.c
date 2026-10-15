/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/order/idempotency.c
 *
 * PURPOSE:
 *   Apply Order Idempotency without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/broker_connectivity/order/idempotency.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_order_idempotency(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "order.idempotency",
        "Order Idempotency",
        UMI_BROKER_POLICY_INTEGRITY,
        100.000000,
        95.000000,
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
UMI_DEFINE_POLICY_BATCH(umi_broker_connectivity_evaluate_order_idempotency_batch, umi_broker_connectivity_evaluate_order_idempotency,
    UmiBrokerConnectivityInput, UmiBrokerConnectivitySnapshot)
