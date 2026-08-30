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

/* BEGINNER NOTE:
 * The output reservation prevents retrieved material from consuming the entire context and leaving no room for a model response.
 */

#include "umicom/ai/context.h"
#include <stddef.h>

UmiStatus umi_ai_context_configure(UmiAiContext *context,
                                   uint32_t limit_tokens,
                                   uint32_t reserved_output_tokens)
{
    if (context == NULL || limit_tokens == 0U || reserved_output_tokens > limit_tokens) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    context->limit_tokens = limit_tokens;
    context->used_tokens = 0U;
    context->reserved_output_tokens = reserved_output_tokens;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_context_consume(UmiAiContext *context, uint32_t tokens)
{
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (tokens > umi_ai_context_remaining(context)) return UMI_STATUS_CAPACITY_EXCEEDED;
    context->used_tokens += tokens;
    return UMI_STATUS_OK;
}

uint32_t umi_ai_context_remaining(const UmiAiContext *context)
{
    uint32_t committed;
    if (context == NULL) return 0U;
    committed = context->used_tokens + context->reserved_output_tokens;
    return committed >= context->limit_tokens ? 0U : context->limit_tokens - committed;
}
