/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/idempotency.h
 *
 * PURPOSE:
 *   Declare an idempotency registry that remembers stable request keys, outcomes and result text for safe command retries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_IDEMPOTENCY_H
#define UMICOM_MESSAGING_IDEMPOTENCY_H

#include <stddef.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the idempotency store data shared with callers of this public contract.
 */
typedef struct UmiIdempotencyStore UmiIdempotencyStore;

/**
 * Initialise idempotency store from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_idempotency_store_create(size_t capacity,
                                       UmiIdempotencyStore **out_store);
/**
 * Release or reset state held by idempotency store so the same storage can be reused
 * safely.
 */
void umi_idempotency_store_destroy(UmiIdempotencyStore *store);
/**
 * Provide the idempotency store record operation used by this module and its client
 * applications.
 */
UmiStatus umi_idempotency_store_record(UmiIdempotencyStore *store,
                                       const char *key,
                                       UmiStatus outcome,
                                       const char *result_text);
/**
 * Find idempotency store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_idempotency_store_lookup(const UmiIdempotencyStore *store,
                                       const char *key,
                                       UmiStatus *out_outcome,
                                       char *out_result,
                                       size_t result_capacity);
/**
 * Return the number of records represented by idempotency store without changing their
 * state.
 */
size_t umi_idempotency_store_count(const UmiIdempotencyStore *store);

#ifdef __cplusplus
}
#endif

#endif
