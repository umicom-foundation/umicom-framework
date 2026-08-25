/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/request_deadline.c
 *
 * PURPOSE:
 *   Implement enforce request deadlines without provider-specific timeout logic in applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/request_deadline.h"
#include <string.h>
void umi_language_intelligence_request_deadline_init(UmiLanguageIntelligenceRequestDeadline *budget, uint64_t limit)
{
    if (budget == NULL) return;
    (void)memset(budget, 0, sizeof(*budget));
    budget->struct_size = (uint32_t)sizeof(*budget);
    budget->api_version = UMI_LANGUAGE_INTELLIGENCE_REQUEST_DEADLINE_API_VERSION;
    budget->limit = limit;
    budget->revision = 1U;
}
uint64_t umi_language_intelligence_request_deadline_remaining(const UmiLanguageIntelligenceRequestDeadline *budget)
{
    if (budget == NULL || budget->used >= budget->limit) return 0U;
    return budget->limit - budget->used;
}
UmiStatus umi_language_intelligence_request_deadline_consume(UmiLanguageIntelligenceRequestDeadline *budget, uint64_t amount)
{
    if (budget == NULL || budget->struct_size < sizeof(*budget) ||
        budget->api_version != UMI_LANGUAGE_INTELLIGENCE_REQUEST_DEADLINE_API_VERSION)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (amount > umi_language_intelligence_request_deadline_remaining(budget)) {
        budget->exhausted = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    budget->used += amount;
    if (budget->used > budget->peak) budget->peak = budget->used;
    budget->exhausted = budget->used >= budget->limit;
    if (budget->revision != UINT64_MAX) budget->revision += 1U;
    return UMI_STATUS_OK;
}
void umi_language_intelligence_request_deadline_reset(UmiLanguageIntelligenceRequestDeadline *budget)
{
    if (budget == NULL) return;
    budget->used = 0U;
    budget->exhausted = 0;
    if (budget->revision != UINT64_MAX) budget->revision += 1U;
}
