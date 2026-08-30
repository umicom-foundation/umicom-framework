/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/symbol_search.h
 *
 * PURPOSE:
 *   Evaluate symbol-search matches with exact/prefix/substring ranking.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_SYMBOL_SEARCH_H
#define UMICOM_LANGUAGE_INTELLIGENCE_SYMBOL_SEARCH_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_SYMBOL_SEARCH_API_VERSION 1U
typedef struct UmiLanguageIntelligenceSymbolSearch {
    uint32_t struct_size;
    uint32_t api_version;
    char text[UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY];
    char scope_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    size_t limit;
    int case_sensitive;
} UmiLanguageIntelligenceSymbolSearch;
void umi_language_intelligence_symbol_search_init(UmiLanguageIntelligenceSymbolSearch *query, const char *text);
uint64_t umi_language_intelligence_symbol_search_candidate_score(
    const UmiLanguageIntelligenceSymbolSearch *query,
    const char *candidate);
int umi_language_intelligence_symbol_search_matches(
    const UmiLanguageIntelligenceSymbolSearch *query,
    const char *candidate);
#ifdef __cplusplus
}
#endif
#endif
