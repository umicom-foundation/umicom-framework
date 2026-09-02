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

/*
 * Initialise ai coding cancellation from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_coding_cancellation_init(UmiAiCodingCancellation *token)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (token != NULL) atomic_init(&token->cancelled, 0);
}

/*
 * Provide the ai coding cancellation request operation used by this module and its client
 * applications.
 */
void umi_ai_coding_cancellation_request(UmiAiCodingCancellation *token)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (token != NULL) {
        atomic_store_explicit(
            &token->cancelled, 1, memory_order_release);
    }
}

/*
 * Provide the ai coding cancellation is requested operation used by this module and its
 * client applications.
 */
int umi_ai_coding_cancellation_is_requested(
    const UmiAiCodingCancellation *token)
{
    return token != NULL &&
        atomic_load_explicit(
            &token->cancelled, memory_order_acquire) != 0;
}
