/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/token_budget.h
 *
 * PURPOSE:
 *   Estimate and reserve context tokens before dispatching a model request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This is deliberately an estimate; provider-specific tokenizers may refine it, but the Framework can still reject obviously impossible requests early.
 */

#ifndef INCLUDE_UMICOM_AI_TOKEN_BUDGET_H
#define INCLUDE_UMICOM_AI_TOKEN_BUDGET_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai token budget data shared with callers of this public contract.
 */
typedef struct UmiAiTokenBudget {
    uint32_t context_limit;
    uint32_t reserved_output;
    uint32_t estimated_input;
} UmiAiTokenBudget;

/**
 * Provide the ai estimate tokens operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_estimate_tokens(const char *utf8_text);
/**
 * Copy ai token budget into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_ai_token_budget_set(UmiAiTokenBudget *budget,
                                  uint32_t context_limit,
                                  uint32_t reserved_output,
                                  const char *input_text);
/**
 * Provide the ai token budget available operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_token_budget_available(const UmiAiTokenBudget *budget);

#ifdef __cplusplus
}
#endif

#endif
