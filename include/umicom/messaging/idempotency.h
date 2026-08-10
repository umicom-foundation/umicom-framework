/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/idempotency.h
 *
 * PURPOSE:
 *   Declare an idempotency registry that remembers stable request keys, outcomes and result text for safe command retries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_IDEMPOTENCY_H
#define UMICOM_MESSAGING_IDEMPOTENCY_H

#include <stddef.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIdempotencyStore UmiIdempotencyStore;

UmiStatus umi_idempotency_store_create(size_t capacity,
                                       UmiIdempotencyStore **out_store);
void umi_idempotency_store_destroy(UmiIdempotencyStore *store);
UmiStatus umi_idempotency_store_record(UmiIdempotencyStore *store,
                                       const char *key,
                                       UmiStatus outcome,
                                       const char *result_text);
UmiStatus umi_idempotency_store_lookup(const UmiIdempotencyStore *store,
                                       const char *key,
                                       UmiStatus *out_outcome,
                                       char *out_result,
                                       size_t result_capacity);
size_t umi_idempotency_store_count(const UmiIdempotencyStore *store);

#ifdef __cplusplus
}
#endif

#endif
