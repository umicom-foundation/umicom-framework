/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/execution/average_price.c
 *
 * PURPOSE:
 *   Apply Execution Average Price without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/execution/average_price.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_execution_average_price(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "execution.average-price",
        "Execution Average Price",
        UMI_BROKER_POLICY_VALUE,
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
