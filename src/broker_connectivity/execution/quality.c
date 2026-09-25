/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/execution/quality.c
 *
 * PURPOSE:
 *   Apply Execution Quality without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/execution/quality.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_execution_quality(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "execution.quality",
        "Execution Quality",
        UMI_BROKER_POLICY_HEALTH,
        85.000000,
        65.000000,
        0.200,
        0.300,
        0.250,
        0.250
    };
    return umi_broker_connectivity_evaluate_policy(
        &policy, input, outSnapshot);
}
