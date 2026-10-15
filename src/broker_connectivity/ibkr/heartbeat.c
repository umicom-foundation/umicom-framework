/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/ibkr/heartbeat.c
 *
 * PURPOSE:
 *   Apply IBKR Heartbeat without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/broker_connectivity/ibkr/heartbeat.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_ibkr_heartbeat(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "ibkr.heartbeat",
        "IBKR Heartbeat",
        UMI_BROKER_POLICY_AGE,
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
UMI_DEFINE_POLICY_BATCH(umi_broker_connectivity_evaluate_ibkr_heartbeat_batch, umi_broker_connectivity_evaluate_ibkr_heartbeat,
    UmiBrokerConnectivityInput, UmiBrokerConnectivitySnapshot)
