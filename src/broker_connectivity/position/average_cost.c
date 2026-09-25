/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/position/average_cost.c
 *
 * PURPOSE:
 *   Apply Position Average Cost without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/position/average_cost.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_position_average_cost(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "position.average-cost",
        "Position Average Cost",
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
