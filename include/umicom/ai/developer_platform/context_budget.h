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

typedef struct UmiAiDevContextBudget { uint64_t limit; uint64_t reserved; uint64_t consumed; uint64_t revision; } UmiAiDevContextBudget;
void umi_ai_dev_context_budget_init(UmiAiDevContextBudget *budget, uint64_t limit);
UmiStatus umi_ai_dev_context_budget_reserve(UmiAiDevContextBudget *budget, uint64_t amount);
UmiStatus umi_ai_dev_context_budget_consume(UmiAiDevContextBudget *budget, uint64_t amount);
uint64_t umi_ai_dev_context_budget_remaining(const UmiAiDevContextBudget *budget);
uint32_t umi_ai_dev_context_budget_utilisation(const UmiAiDevContextBudget *budget);

#ifdef __cplusplus
}
#endif

#endif
