/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/ibkr/client_id.c
 *
 * PURPOSE:
 *   Apply IBKR Client Id without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/broker_connectivity/ibkr/client_id.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_ibkr_client_id(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "ibkr.client-id",
        "IBKR Client Id",
        UMI_BROKER_POLICY_READINESS,
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
UMI_DEFINE_POLICY_BATCH(umi_broker_connectivity_evaluate_ibkr_client_id_batch, umi_broker_connectivity_evaluate_ibkr_client_id,
    UmiBrokerConnectivityInput, UmiBrokerConnectivitySnapshot)
