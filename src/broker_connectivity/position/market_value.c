/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/position/market_value.c
 *
 * PURPOSE:
 *   Apply Position Market Value without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/position/market_value.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_position_market_value(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "position.market-value",
        "Position Market Value",
        UMI_BROKER_POLICY_NOTIONAL,
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
