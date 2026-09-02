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

/**
 * Represent the cancellation token data shared with callers of this public contract.
 */
typedef struct UmiCancellationToken UmiCancellationToken;

/**
 * Initialise cancellation token from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_cancellation_token_create(UmiCancellationToken **out_token);
/**
 * Release or reset state held by cancellation token so the same storage can be reused
 * safely.
 */
void umi_cancellation_token_destroy(UmiCancellationToken *token);
/**
 * Provide the cancellation token request operation used by this module and its client
 * applications.
 */
void umi_cancellation_token_request(UmiCancellationToken *token);
/**
 * Release or reset state held by cancellation token so the same storage can be reused
 * safely.
 */
void umi_cancellation_token_reset(UmiCancellationToken *token);
/**
 * Provide the cancellation token is requested operation used by this module and its client
 * applications.
 */
int umi_cancellation_token_is_requested(const UmiCancellationToken *token);
/**
 * Provide the cancellation token generation operation used by this module and its client
 * applications.
 */
uint64_t umi_cancellation_token_generation(const UmiCancellationToken *token);

#ifdef __cplusplus
}
#endif

#endif
