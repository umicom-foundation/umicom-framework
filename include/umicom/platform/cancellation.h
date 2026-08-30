/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cancellation.h
 *
 * PURPOSE:
 *   Provide a small thread-safe cancellation token shared by processes,
 *   searches, file watchers and future asynchronous Framework operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CANCELLATION_H
#define UMICOM_PLATFORM_CANCELLATION_H

#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCancellationToken UmiCancellationToken;

UmiStatus umi_cancellation_token_create(UmiCancellationToken **out_token);
void umi_cancellation_token_destroy(UmiCancellationToken *token);
void umi_cancellation_token_request(UmiCancellationToken *token);
void umi_cancellation_token_reset(UmiCancellationToken *token);
int umi_cancellation_token_is_requested(const UmiCancellationToken *token);
uint64_t umi_cancellation_token_generation(const UmiCancellationToken *token);

#ifdef __cplusplus
}
#endif

#endif
