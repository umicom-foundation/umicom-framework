/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/token_budget.c
 *
 * PURPOSE:
 *   Provide deterministic approximate token-budget checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Early budgeting prevents a local or remote provider from receiving a request that cannot fit in the selected model context.
 */

#include "umicom/ai/token_budget.h"
#include <stddef.h>

#include <string.h>

uint32_t umi_ai_estimate_tokens(const char *utf8_text)
{
    size_t bytes;
    if (utf8_text == NULL || utf8_text[0] == '\0') {
        return 0U;
    }
    bytes = strlen(utf8_text);
    return (uint32_t)((bytes + 3U) / 4U);
}

UmiStatus umi_ai_token_budget_set(UmiAiTokenBudget *budget,
                                  uint32_t context_limit,
                                  uint32_t reserved_output,
                                  const char *input_text)
{
    uint32_t estimated;
    if (budget == NULL || input_text == NULL ||
        context_limit == 0U || reserved_output > context_limit) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    estimated = umi_ai_estimate_tokens(input_text);
    if (estimated > context_limit - reserved_output) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    budget->context_limit = context_limit;
    budget->reserved_output = reserved_output;
    budget->estimated_input = estimated;
    return UMI_STATUS_OK;
}

uint32_t umi_ai_token_budget_available(const UmiAiTokenBudget *budget)
{
    if (budget == NULL ||
        budget->estimated_input + budget->reserved_output >= budget->context_limit) {
        return 0U;
    }
    return budget->context_limit - budget->estimated_input - budget->reserved_output;
}
