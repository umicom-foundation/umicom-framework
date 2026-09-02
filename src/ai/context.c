/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/context.c
 *
 * PURPOSE:
 *   Track context-window consumption.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The output reservation prevents retrieved material from consuming the entire context and leaving no room for a model response.
 */

#include "umicom/ai/context.h"
#include <stddef.h>

/*
 * Provide the ai context configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_context_configure(UmiAiContext *context,
                                   uint32_t limit_tokens,
                                   uint32_t reserved_output_tokens)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || limit_tokens == 0U || reserved_output_tokens > limit_tokens) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    context->limit_tokens = limit_tokens;
    context->used_tokens = 0U;
    context->reserved_output_tokens = reserved_output_tokens;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai context consume operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_context_consume(UmiAiContext *context, uint32_t tokens)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (tokens > umi_ai_context_remaining(context)) return UMI_STATUS_CAPACITY_EXCEEDED;
    context->used_tokens += tokens;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai context remaining operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_context_remaining(const UmiAiContext *context)
{
    uint32_t committed;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL) return 0U;
    committed = context->used_tokens + context->reserved_output_tokens;
    return committed >= context->limit_tokens ? 0U : context->limit_tokens - committed;
}
