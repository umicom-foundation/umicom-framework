/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/audit/gap.c
 *
 * PURPOSE:
 *   Apply Audit Sequence Gap without duplicating canonical trading or broker state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/broker_connectivity/audit/gap.h"
#include "../broker_connectivity_internal.h"

UmiStatus umi_broker_connectivity_evaluate_audit_gap(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    static const UmiBrokerConnectivityPolicy policy = {
        "audit.gap",
        "Audit Sequence Gap",
        UMI_BROKER_POLICY_GAP,
        0.000000,
        1.000000,
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
UMI_DEFINE_POLICY_BATCH(umi_broker_connectivity_evaluate_audit_gap_batch, umi_broker_connectivity_evaluate_audit_gap,
    UmiBrokerConnectivityInput, UmiBrokerConnectivitySnapshot)
