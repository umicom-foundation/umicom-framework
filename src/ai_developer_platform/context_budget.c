/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/context_budget.c
 *
 * PURPOSE:
 *   Allocate bounded context capacity across evidence categories.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/context_budget.h"
void umi_ai_dev_context_budget_init(UmiAiDevContextBudget *budget, uint64_t limit) {
    if (budget == NULL) {
        return;
    }
    budget->limit = limit;
    budget->reserved = 0U;
    budget->consumed = 0U;
    budget->revision = 1U;
}
uint64_t umi_ai_dev_context_budget_remaining(const UmiAiDevContextBudget *budget) {
    uint64_t used;
    if (budget == NULL) {
        return 0U;
    }
    used = budget->reserved + budget->consumed;
    return used >= budget->limit ? 0U : budget->limit - used;
}
UmiStatus umi_ai_dev_context_budget_reserve(UmiAiDevContextBudget *budget, uint64_t amount) {
    if (budget == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (amount > umi_ai_dev_context_budget_remaining(budget)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    budget->reserved += amount;
    ++budget->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_ai_dev_context_budget_consume(UmiAiDevContextBudget *budget, uint64_t amount) {
    if (budget == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (amount > budget->reserved + umi_ai_dev_context_budget_remaining(budget)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (amount <= budget->reserved) {
        budget->reserved -= amount;
    } else {
        budget->reserved = 0U;
    }
    budget->consumed += amount;
    ++budget->revision;
    return UMI_STATUS_OK;
}
uint32_t umi_ai_dev_context_budget_utilisation(const UmiAiDevContextBudget *budget) {
    if (budget == NULL) {
        return 0U;
    }
    return umi_ai_dev_percent(budget->consumed + budget->reserved, budget->limit);
}
