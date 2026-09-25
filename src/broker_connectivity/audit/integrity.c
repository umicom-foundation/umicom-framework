/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/audit/integrity.c
 *
 * PURPOSE:
 *   Apply Audit Integrity without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/audit/integrity.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_audit_integrity(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "audit.integrity",
        "Audit Integrity",
        UMI_BROKER_POLICY_INTEGRITY,
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
