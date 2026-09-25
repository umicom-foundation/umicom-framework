/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/order/ack_latency.c
 *
 * PURPOSE:
 *   Apply Order Ack Latency without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/order/ack_latency.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_order_ack_latency(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "order.ack-latency",
        "Order Ack Latency",
        UMI_BROKER_POLICY_LATENCY,
        0.000000,
        500.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_broker_connectivity_evaluate_policy(
        &policy, input, outSnapshot);
}
