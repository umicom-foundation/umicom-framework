/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/model_budget.h
 *
 * PURPOSE:
 *   Enforce per-request and session token/cost budgets.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MODEL_BUDGET_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MODEL_BUDGET_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev model budget data shared with callers of this public contract.
 */
typedef struct UmiAiDevModelBudget { uint64_t limit; uint64_t reserved; uint64_t consumed; uint64_t revision; } UmiAiDevModelBudget;
/**
 * Initialise ai dev model budget from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_dev_model_budget_init(UmiAiDevModelBudget *budget, uint64_t limit);
/**
 * Provide the ai dev model budget reserve operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_model_budget_reserve(UmiAiDevModelBudget *budget, uint64_t amount);
/**
 * Provide the ai dev model budget consume operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_model_budget_consume(UmiAiDevModelBudget *budget, uint64_t amount);
/**
 * Provide the ai dev model budget remaining operation used by this module and its client
 * applications.
 */
uint64_t umi_ai_dev_model_budget_remaining(const UmiAiDevModelBudget *budget);
/**
 * Provide the ai dev model budget utilisation operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_model_budget_utilisation(const UmiAiDevModelBudget *budget);

#ifdef __cplusplus
}
#endif

#endif
