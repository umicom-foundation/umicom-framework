/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/performance_budget.h
 *
 * PURPOSE:
 *   Track latency/performance budget consumption for language services.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_PERFORMANCE_BUDGET_H
#define UMICOM_LANGUAGE_INTELLIGENCE_PERFORMANCE_BUDGET_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_PERFORMANCE_BUDGET_API_VERSION 1U
typedef struct UmiLanguageIntelligencePerformanceBudget {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t limit;
    uint64_t used;
    uint64_t peak;
    uint64_t revision;
    int exhausted;
} UmiLanguageIntelligencePerformanceBudget;
void umi_language_intelligence_performance_budget_init(UmiLanguageIntelligencePerformanceBudget *budget, uint64_t limit);
UmiStatus umi_language_intelligence_performance_budget_consume(UmiLanguageIntelligencePerformanceBudget *budget, uint64_t amount);
uint64_t umi_language_intelligence_performance_budget_remaining(const UmiLanguageIntelligencePerformanceBudget *budget);
void umi_language_intelligence_performance_budget_reset(UmiLanguageIntelligencePerformanceBudget *budget);
#ifdef __cplusplus
}
#endif
#endif
