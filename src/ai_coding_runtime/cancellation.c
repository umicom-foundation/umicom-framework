/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/cancellation.c
 *
 * PURPOSE:
 *   Implement lock-free cancellation signalling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/cancellation.h"

void umi_ai_coding_cancellation_init(UmiAiCodingCancellation *token)
{
    if (token != NULL) atomic_init(&token->cancelled, 0);
}

void umi_ai_coding_cancellation_request(UmiAiCodingCancellation *token)
{
    if (token != NULL) {
        atomic_store_explicit(
            &token->cancelled, 1, memory_order_release);
    }
}

int umi_ai_coding_cancellation_is_requested(
    const UmiAiCodingCancellation *token)
{
    return token != NULL &&
        atomic_load_explicit(
            &token->cancelled, memory_order_acquire) != 0;
}
