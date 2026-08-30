/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/idempotency_policy.h
 *
 * PURPOSE:
 *   Determine when an operation requires a stable idempotency key before retries are permitted.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_IDEMPOTENCY_POLICY_H
#define UMICOM_INTEGRATION_FABRIC_IDEMPOTENCY_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricIdempotencyPolicy { bool require_key_for_retry; bool require_key_for_write; size_t minimum_key_length; } UmiFabricIdempotencyPolicy;
void umi_fabric_idempotency_policy_default(UmiFabricIdempotencyPolicy *policy);
UmiStatus umi_fabric_idempotency_policy_evaluate(const UmiFabricIdempotencyPolicy *policy,bool write,bool retry,const char *key,bool *out_allowed);

#ifdef __cplusplus
}
#endif
#endif
