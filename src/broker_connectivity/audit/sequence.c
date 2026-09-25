/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/audit/sequence.c
 *
 * PURPOSE:
 *   Apply Audit Sequence Integrity without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/audit/sequence.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_audit_sequence(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "audit.sequence",
        "Audit Sequence Integrity",
        UMI_BROKER_POLICY_SEQUENCE,
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
