/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/audit/readiness.c
 *
 * PURPOSE:
 *   Apply Audit Readiness without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/audit/readiness.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_audit_readiness(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "audit.readiness",
        "Audit Readiness",
        UMI_BROKER_POLICY_HEALTH,
        90.000000,
        70.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_broker_connectivity_evaluate_policy(
        &policy, input, outSnapshot);
}
