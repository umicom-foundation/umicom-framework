/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/model_context_window.c
 *
 * PURPOSE:
 *   Reserve context-window capacity for instructions, evidence and response tokens.
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
#include "umicom/ai/developer_platform/model_context_window.h"
void umi_ai_dev_model_context_window_init(UmiAiDevModelContextWindow *budget, uint64_t limit) {
    if (budget == NULL) {
        return;
    }
    budget->limit = limit;
    budget->reserved = 0U;
    budget->consumed = 0U;
    budget->revision = 1U;
}
uint64_t umi_ai_dev_model_context_window_remaining(const UmiAiDevModelContextWindow *budget) {
    uint64_t used;
    if (budget == NULL) {
        return 0U;
    }
    used = budget->reserved + budget->consumed;
    return used >= budget->limit ? 0U : budget->limit - used;
}
UmiStatus umi_ai_dev_model_context_window_reserve(UmiAiDevModelContextWindow *budget, uint64_t amount) {
    if (budget == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (amount > umi_ai_dev_model_context_window_remaining(budget)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    budget->reserved += amount;
    ++budget->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_ai_dev_model_context_window_consume(UmiAiDevModelContextWindow *budget, uint64_t amount) {
    if (budget == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (amount > budget->reserved + umi_ai_dev_model_context_window_remaining(budget)) {
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
uint32_t umi_ai_dev_model_context_window_utilisation(const UmiAiDevModelContextWindow *budget) {
    if (budget == NULL) {
        return 0U;
    }
    return umi_ai_dev_percent(budget->consumed + budget->reserved, budget->limit);
}
