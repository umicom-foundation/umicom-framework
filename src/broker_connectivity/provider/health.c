/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/provider/health.c
 *
 * PURPOSE:
 *   Apply Provider Health without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/broker_connectivity/provider/health.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_provider_health(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "provider.health",
        "Provider Health",
        UMI_BROKER_POLICY_HEALTH,
        85.000000,
        65.000000,
        0.200,
        0.250,
        0.250,
        0.300
    };
    return umi_broker_connectivity_evaluate_policy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_broker_connectivity_evaluate_provider_health_batch, umi_broker_connectivity_evaluate_provider_health,
    UmiBrokerConnectivityInput, UmiBrokerConnectivitySnapshot)
