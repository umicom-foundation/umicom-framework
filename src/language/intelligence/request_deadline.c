/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/request_deadline.c
 *
 * PURPOSE:
 *   Implement enforce request deadlines without provider-specific timeout logic in applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/request_deadline.h"
#include <string.h>
/*
 * Initialise language intelligence request deadline from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_request_deadline_init(UmiLanguageIntelligenceRequestDeadline *budget, uint64_t limit)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL) return;
    (void)memset(budget, 0, sizeof(*budget));
    budget->struct_size = (uint32_t)sizeof(*budget);
    budget->api_version = UMI_LANGUAGE_INTELLIGENCE_REQUEST_DEADLINE_API_VERSION;
    budget->limit = limit;
    budget->revision = 1U;
}
/*
 * Provide the language intelligence request deadline remaining operation used by this
 * module and its client applications.
 */
uint64_t umi_language_intelligence_request_deadline_remaining(const UmiLanguageIntelligenceRequestDeadline *budget)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL || budget->used >= budget->limit) return 0U;
    return budget->limit - budget->used;
}
/*
 * Provide the language intelligence request deadline consume operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_intelligence_request_deadline_consume(UmiLanguageIntelligenceRequestDeadline *budget, uint64_t amount)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (budget == NULL || budget->struct_size < sizeof(*budget) ||
        budget->api_version != UMI_LANGUAGE_INTELLIGENCE_REQUEST_DEADLINE_API_VERSION)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (amount > umi_language_intelligence_request_deadline_remaining(budget)) {
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
 * Release or reset state held by language intelligence request deadline so the same
 * storage can be reused safely.
 */
void umi_language_intelligence_request_deadline_reset(UmiLanguageIntelligenceRequestDeadline *budget)
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
