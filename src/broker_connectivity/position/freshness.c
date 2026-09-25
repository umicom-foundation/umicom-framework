/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/position/freshness.c
 *
 * PURPOSE:
 *   Apply Position Freshness without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/position/freshness.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_position_freshness(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "position.freshness",
        "Position Freshness",
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
