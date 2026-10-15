/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/ibkr/status_mapping.c
 *
 * PURPOSE:
 *   Apply IBKR Status Mapping without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/broker_connectivity/ibkr/status_mapping.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_ibkr_status_mapping(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "ibkr.status-mapping",
        "IBKR Status Mapping",
        UMI_BROKER_POLICY_COVERAGE,
        100.000000,
        95.000000,
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
UMI_DEFINE_POLICY_BATCH(umi_broker_connectivity_evaluate_ibkr_status_mapping_batch, umi_broker_connectivity_evaluate_ibkr_status_mapping,
    UmiBrokerConnectivityInput, UmiBrokerConnectivitySnapshot)
