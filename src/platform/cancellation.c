/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cancellation.c
 *
 * PURPOSE:
 *   Implement cancellation using C atomics so a request can safely cross
 *   worker, process, watcher and search threads without platform types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/cancellation.h"

#include <stdatomic.h>
#include <stdlib.h>

struct UmiCancellationToken {
    atomic_int requested;
    atomic_uint_fast64_t generation;
};

/*
 * Initialise cancellation token from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_cancellation_token_create(UmiCancellationToken **out_token)
{
    UmiCancellationToken *token;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_token == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_token = NULL;
    token = (UmiCancellationToken *)calloc(1U, sizeof(*token));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (token == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    atomic_init(&token->requested, 0);
    atomic_init(&token->generation, 1U);
    *out_token = token;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by cancellation token so the same storage can be reused
 * safely.
 */
void umi_cancellation_token_destroy(UmiCancellationToken *token)
{
    free(token);
}

/*
 * Provide the cancellation token request operation used by this module and its client
 * applications.
 */
void umi_cancellation_token_request(UmiCancellationToken *token)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (token == NULL) return;
    atomic_store(&token->requested, 1);
    (void)atomic_fetch_add(&token->generation, 1U);
}

/*
 * Release or reset state held by cancellation token so the same storage can be reused
 * safely.
 */
void umi_cancellation_token_reset(UmiCancellationToken *token)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (token == NULL) return;
    atomic_store(&token->requested, 0);
    (void)atomic_fetch_add(&token->generation, 1U);
}

/*
 * Provide the cancellation token is requested operation used by this module and its client
 * applications.
 */
int umi_cancellation_token_is_requested(const UmiCancellationToken *token)
{
    return token != NULL && atomic_load(&token->requested) != 0;
}

/*
 * Provide the cancellation token generation operation used by this module and its client
 * applications.
 */
uint64_t umi_cancellation_token_generation(const UmiCancellationToken *token)
{
    return token != NULL ? atomic_load(&token->generation) : 0U;
}
