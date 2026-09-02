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

/*
 * Early budgeting prevents a local or remote provider from receiving a request that cannot fit in the selected model context.
 */

#include "umicom/ai/token_budget.h"
#include <stddef.h>

#include <string.h>

/*
 * Provide the ai estimate tokens operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_estimate_tokens(const char *utf8_text)
{
    size_t bytes;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (utf8_text == NULL || utf8_text[0] == '\0') {
        return 0U;
    }
    bytes = strlen(utf8_text);
    return (uint32_t)((bytes + 3U) / 4U);
}

/*
 * Copy ai token budget into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_ai_token_budget_set(UmiAiTokenBudget *budget,
                                  uint32_t context_limit,
                                  uint32_t reserved_output,
                                  const char *input_text)
{
    uint32_t estimated;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL || input_text == NULL ||
        context_limit == 0U || reserved_output > context_limit) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    estimated = umi_ai_estimate_tokens(input_text);
    /* Apply this branch only when its contract condition is satisfied. */
    if (estimated > context_limit - reserved_output) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    budget->context_limit = context_limit;
    budget->reserved_output = reserved_output;
    budget->estimated_input = estimated;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai token budget available operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_token_budget_available(const UmiAiTokenBudget *budget)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL ||
        budget->estimated_input + budget->reserved_output >= budget->context_limit) {
        return 0U;
    }
    return budget->context_limit - budget->estimated_input - budget->reserved_output;
}
