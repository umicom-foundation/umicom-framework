/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/context_budget.h
 *
 * PURPOSE:
 *   Allocate bounded context capacity across evidence categories.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_BUDGET_H
#define UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_BUDGET_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev context budget data shared with callers of this public contract.
 */
typedef struct UmiAiDevContextBudget { uint64_t limit; uint64_t reserved; uint64_t consumed; uint64_t revision; } UmiAiDevContextBudget;
/**
 * Initialise ai dev context budget from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_dev_context_budget_init(UmiAiDevContextBudget *budget, uint64_t limit);
/**
 * Provide the ai dev context budget reserve operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_context_budget_reserve(UmiAiDevContextBudget *budget, uint64_t amount);
/**
 * Provide the ai dev context budget consume operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_context_budget_consume(UmiAiDevContextBudget *budget, uint64_t amount);
/**
 * Provide the ai dev context budget remaining operation used by this module and its client
 * applications.
 */
uint64_t umi_ai_dev_context_budget_remaining(const UmiAiDevContextBudget *budget);
/**
 * Provide the ai dev context budget utilisation operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_context_budget_utilisation(const UmiAiDevContextBudget *budget);

#ifdef __cplusplus
}
#endif

#endif
