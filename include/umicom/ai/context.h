/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/context.h
 *
 * PURPOSE:
 *   Track context usage and reserved output for one model request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Context accounting is provider-neutral, letting Studio enforce the same budgeting rules for local and remote models.
 */

#ifndef INCLUDE_UMICOM_AI_CONTEXT_H
#define INCLUDE_UMICOM_AI_CONTEXT_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai context data shared with callers of this public contract.
 */
typedef struct UmiAiContext {
    uint32_t limit_tokens;
    uint32_t used_tokens;
    uint32_t reserved_output_tokens;
} UmiAiContext;

/**
 * Provide the ai context configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_context_configure(UmiAiContext *context,
                                   uint32_t limit_tokens,
                                   uint32_t reserved_output_tokens);
/**
 * Provide the ai context consume operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_context_consume(UmiAiContext *context, uint32_t tokens);
/**
 * Provide the ai context remaining operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_context_remaining(const UmiAiContext *context);

#ifdef __cplusplus
}
#endif

#endif
