/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/provider/contract.c
 *
 * PURPOSE:
 *   Apply Provider Contract without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/broker_connectivity/provider/contract.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_provider_contract(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "provider.contract",
        "Provider Contract",
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
UMI_DEFINE_POLICY_BATCH(umi_broker_connectivity_evaluate_provider_contract_batch, umi_broker_connectivity_evaluate_provider_contract,
    UmiBrokerConnectivityInput, UmiBrokerConnectivitySnapshot)
