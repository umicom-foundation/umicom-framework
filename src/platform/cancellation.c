/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cancellation.c
 *
 * PURPOSE:
 *   Implement cancellation using C atomics so a request can safely cross
 *   worker, process, watcher and search threads without platform types.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/cancellation.h"

#include <stdatomic.h>
#include <stdlib.h>

struct UmiCancellationToken {
    atomic_int requested;
    atomic_uint_fast64_t generation;
};

UmiStatus umi_cancellation_token_create(UmiCancellationToken **out_token)
{
    UmiCancellationToken *token;
    if (out_token == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_token = NULL;
    token = (UmiCancellationToken *)calloc(1U, sizeof(*token));
    if (token == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    atomic_init(&token->requested, 0);
    atomic_init(&token->generation, 1U);
    *out_token = token;
    return UMI_STATUS_OK;
}

void umi_cancellation_token_destroy(UmiCancellationToken *token)
{
    free(token);
}

void umi_cancellation_token_request(UmiCancellationToken *token)
{
    if (token == NULL) return;
    atomic_store(&token->requested, 1);
    (void)atomic_fetch_add(&token->generation, 1U);
}

void umi_cancellation_token_reset(UmiCancellationToken *token)
{
    if (token == NULL) return;
    atomic_store(&token->requested, 0);
    (void)atomic_fetch_add(&token->generation, 1U);
}

int umi_cancellation_token_is_requested(const UmiCancellationToken *token)
{
    return token != NULL && atomic_load(&token->requested) != 0;
}

uint64_t umi_cancellation_token_generation(const UmiCancellationToken *token)
{
    return token != NULL ? atomic_load(&token->generation) : 0U;
}
