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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiAiDevModelBudget { uint64_t limit; uint64_t reserved; uint64_t consumed; uint64_t revision; } UmiAiDevModelBudget;
void umi_ai_dev_model_budget_init(UmiAiDevModelBudget *budget, uint64_t limit);
UmiStatus umi_ai_dev_model_budget_reserve(UmiAiDevModelBudget *budget, uint64_t amount);
UmiStatus umi_ai_dev_model_budget_consume(UmiAiDevModelBudget *budget, uint64_t amount);
uint64_t umi_ai_dev_model_budget_remaining(const UmiAiDevModelBudget *budget);
uint32_t umi_ai_dev_model_budget_utilisation(const UmiAiDevModelBudget *budget);

#ifdef __cplusplus
}
#endif

#endif
