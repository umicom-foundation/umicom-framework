/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/request_budget.h
 *
 * PURPOSE:
 *   Track per-request work budgets for CPU/operation units.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_BUDGET_H
#define UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_BUDGET_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_REQUEST_BUDGET_API_VERSION 1U
typedef struct UmiLanguageIntelligenceRequestBudget {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t limit;
    uint64_t used;
    uint64_t peak;
    uint64_t revision;
    int exhausted;
} UmiLanguageIntelligenceRequestBudget;
void umi_language_intelligence_request_budget_init(UmiLanguageIntelligenceRequestBudget *budget, uint64_t limit);
UmiStatus umi_language_intelligence_request_budget_consume(UmiLanguageIntelligenceRequestBudget *budget, uint64_t amount);
uint64_t umi_language_intelligence_request_budget_remaining(const UmiLanguageIntelligenceRequestBudget *budget);
void umi_language_intelligence_request_budget_reset(UmiLanguageIntelligenceRequestBudget *budget);
#ifdef __cplusplus
}
#endif
#endif
