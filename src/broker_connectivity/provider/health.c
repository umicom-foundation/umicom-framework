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
