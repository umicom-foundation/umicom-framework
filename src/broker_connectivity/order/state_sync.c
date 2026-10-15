/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/order/state_sync.c
 *
 * PURPOSE:
 *   Apply Order State Synchronisation without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/broker_connectivity/order/state_sync.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_order_state_sync(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "order.state-sync",
        "Order State Synchronisation",
        UMI_BROKER_POLICY_FRESHNESS,
        0.000000,
        5000.000000,
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
UMI_DEFINE_POLICY_BATCH(umi_broker_connectivity_evaluate_order_state_sync_batch, umi_broker_connectivity_evaluate_order_state_sync,
    UmiBrokerConnectivityInput, UmiBrokerConnectivitySnapshot)
