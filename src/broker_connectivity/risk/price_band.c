/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/risk/price_band.c
 *
 * PURPOSE:
 *   Apply Price Band Gate without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/broker_connectivity/risk/price_band.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_risk_price_band(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "risk.price-band",
        "Price Band Gate",
        UMI_BROKER_POLICY_PRICE_BAND,
        0.000000,
        10.000000,
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
UMI_DEFINE_POLICY_BATCH(umi_broker_connectivity_evaluate_risk_price_band_batch, umi_broker_connectivity_evaluate_risk_price_band,
    UmiBrokerConnectivityInput, UmiBrokerConnectivitySnapshot)
