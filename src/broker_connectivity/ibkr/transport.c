/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/ibkr/transport.c
 *
 * PURPOSE:
 *   Apply IBKR Transport Readiness without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/broker_connectivity/ibkr/transport.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_ibkr_transport(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "ibkr.transport",
        "IBKR Transport Readiness",
        UMI_BROKER_POLICY_CONNECTION,
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
UMI_DEFINE_POLICY_BATCH(umi_broker_connectivity_evaluate_ibkr_transport_batch, umi_broker_connectivity_evaluate_ibkr_transport,
    UmiBrokerConnectivityInput, UmiBrokerConnectivitySnapshot)
