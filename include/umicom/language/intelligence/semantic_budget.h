/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/semantic_budget.h
 *
 * PURPOSE:
 *   Limit semantic analysis work for large or rapidly changing documents.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_SEMANTIC_BUDGET_H
#define UMICOM_LANGUAGE_INTELLIGENCE_SEMANTIC_BUDGET_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_BUDGET_API_VERSION 1U
/**
 * Represent the language intelligence semantic budget data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceSemanticBudget {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t limit;
    uint64_t used;
    uint64_t peak;
    uint64_t revision;
    int exhausted;
} UmiLanguageIntelligenceSemanticBudget;
/**
 * Initialise language intelligence semantic budget from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_semantic_budget_init(UmiLanguageIntelligenceSemanticBudget *budget, uint64_t limit);
/**
 * Provide the language intelligence semantic budget consume operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_intelligence_semantic_budget_consume(UmiLanguageIntelligenceSemanticBudget *budget, uint64_t amount);
/**
 * Provide the language intelligence semantic budget remaining operation used by this
 * module and its client applications.
 */
uint64_t umi_language_intelligence_semantic_budget_remaining(const UmiLanguageIntelligenceSemanticBudget *budget);
/**
 * Release or reset state held by language intelligence semantic budget so the same storage
 * can be reused safely.
 */
void umi_language_intelligence_semantic_budget_reset(UmiLanguageIntelligenceSemanticBudget *budget);
#ifdef __cplusplus
}
#endif
#endif
