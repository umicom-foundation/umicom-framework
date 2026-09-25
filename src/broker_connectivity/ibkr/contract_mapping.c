/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/ibkr/contract_mapping.c
 *
 * PURPOSE:
 *   Apply IBKR Contract Mapping without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/ibkr/contract_mapping.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_ibkr_contract_mapping(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "ibkr.contract-mapping",
        "IBKR Contract Mapping",
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
