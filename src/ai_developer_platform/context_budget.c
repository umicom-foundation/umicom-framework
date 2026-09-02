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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/context_budget.h"
/*
 * Initialise ai dev context budget from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_dev_context_budget_init(UmiAiDevContextBudget *budget, uint64_t limit) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL) {
        return;
    }
    budget->limit = limit;
    budget->reserved = 0U;
    budget->consumed = 0U;
    budget->revision = 1U;
}
/*
 * Provide the ai dev context budget remaining operation used by this module and its client
 * applications.
 */
uint64_t umi_ai_dev_context_budget_remaining(const UmiAiDevContextBudget *budget) {
    uint64_t used;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL) {
        return 0U;
    }
    used = budget->reserved + budget->consumed;
    return used >= budget->limit ? 0U : budget->limit - used;
}
/*
 * Provide the ai dev context budget reserve operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_context_budget_reserve(UmiAiDevContextBudget *budget, uint64_t amount) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (amount > umi_ai_dev_context_budget_remaining(budget)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    budget->reserved += amount;
    ++budget->revision;
    return UMI_STATUS_OK;
}
/*
 * Provide the ai dev context budget consume operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_context_budget_consume(UmiAiDevContextBudget *budget, uint64_t amount) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (amount > budget->reserved + umi_ai_dev_context_budget_remaining(budget)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (amount <= budget->reserved) {
        budget->reserved -= amount;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        budget->reserved = 0U;
    }
    budget->consumed += amount;
    ++budget->revision;
    return UMI_STATUS_OK;
}
/*
 * Provide the ai dev context budget utilisation operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_context_budget_utilisation(const UmiAiDevContextBudget *budget) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL) {
        return 0U;
    }
    return umi_ai_dev_percent(budget->consumed + budget->reserved, budget->limit);
}
