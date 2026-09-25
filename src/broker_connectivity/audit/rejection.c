/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/audit/rejection.c
 *
 * PURPOSE:
 *   Apply Audit Rejection Evidence without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/audit/rejection.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_audit_rejection(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "audit.rejection",
        "Audit Rejection Evidence",
        UMI_BROKER_POLICY_REJECTION,
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
