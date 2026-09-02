/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/performance_budget.c
 *
 * PURPOSE:
 *   Implement track latency/performance budget consumption for language services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/performance_budget.h"
#include <string.h>
/*
 * Initialise language intelligence performance budget from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_performance_budget_init(UmiLanguageIntelligencePerformanceBudget *budget, uint64_t limit)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL) return;
    (void)memset(budget, 0, sizeof(*budget));
    budget->struct_size = (uint32_t)sizeof(*budget);
    budget->api_version = UMI_LANGUAGE_INTELLIGENCE_PERFORMANCE_BUDGET_API_VERSION;
    budget->limit = limit;
    budget->revision = 1U;
}
/*
 * Provide the language intelligence performance budget remaining operation used by this
 * module and its client applications.
 */
uint64_t umi_language_intelligence_performance_budget_remaining(const UmiLanguageIntelligencePerformanceBudget *budget)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL || budget->used >= budget->limit) return 0U;
    return budget->limit - budget->used;
}
/*
 * Provide the language intelligence performance budget consume operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_performance_budget_consume(UmiLanguageIntelligencePerformanceBudget *budget, uint64_t amount)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL || budget->struct_size < sizeof(*budget) ||
        budget->api_version != UMI_LANGUAGE_INTELLIGENCE_PERFORMANCE_BUDGET_API_VERSION)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (amount > umi_language_intelligence_performance_budget_remaining(budget)) {
        budget->exhausted = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    budget->used += amount;
    /* Apply this branch only when its contract condition is satisfied. */
    if (budget->used > budget->peak) budget->peak = budget->used;
    budget->exhausted = budget->used >= budget->limit;
    /* Apply this branch only when its contract condition is satisfied. */
    if (budget->revision != UINT64_MAX) budget->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Release or reset state held by language intelligence performance budget so the same
 * storage can be reused safely.
 */
void umi_language_intelligence_performance_budget_reset(UmiLanguageIntelligencePerformanceBudget *budget)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL) return;
    budget->used = 0U;
    budget->exhausted = 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (budget->revision != UINT64_MAX) budget->revision += 1U;
}
