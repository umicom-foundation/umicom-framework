/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/audit/causation.h
 *
 * PURPOSE:
 *   Evaluate Audit Causation Coverage through the provider-neutral broker connectivity layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_AUDIT_CAUSATION_H
#define UMICOM_BROKER_CONNECTIVITY_AUDIT_CAUSATION_H

#include <stddef.h>
#include "umicom/broker_connectivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_broker_connectivity_evaluate_audit_causation(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot);


/** Evaluate a complete group using this policy without publishing partial rows.
 * Zero count is a no-op and permits NULL arrays. Nonempty groups require arrays
 * with count readable inputs and capacity writable outputs. At most 4096 rows
 * and 16 MiB of temporary output are accepted; allocation can fail.
 * Any failure preserves all outputs. Non-finite signals and overlong labels
 * are refused. Inputs may borrow output storage until publication. The caller
 * serializes access; this calculation does not submit orders or execute jobs.
 */
UmiStatus umi_broker_connectivity_evaluate_audit_causation_batch(const UmiBrokerConnectivityInput *inputs,
    size_t count, UmiBrokerConnectivitySnapshot *outputs, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
