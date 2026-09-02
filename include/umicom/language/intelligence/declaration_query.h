/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/declaration_query.h
 *
 * PURPOSE:
 *   Represent and score declaration-navigation candidates.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_DECLARATION_QUERY_H
#define UMICOM_LANGUAGE_INTELLIGENCE_DECLARATION_QUERY_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_DECLARATION_QUERY_API_VERSION 1U
/**
 * Represent the language intelligence declaration query data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceDeclarationQuery {
    uint32_t struct_size;
    uint32_t api_version;
    char text[UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY];
    char scope_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    size_t limit;
    int case_sensitive;
} UmiLanguageIntelligenceDeclarationQuery;
/**
 * Initialise language intelligence declaration query from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_declaration_query_init(UmiLanguageIntelligenceDeclarationQuery *query, const char *text);
/**
 * Provide the language intelligence declaration query candidate score operation used by
 * this module and its client applications.
 */
uint64_t umi_language_intelligence_declaration_query_candidate_score(
    const UmiLanguageIntelligenceDeclarationQuery *query,
    const char *candidate);
/**
 * Provide the language intelligence declaration query matches operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_declaration_query_matches(
    const UmiLanguageIntelligenceDeclarationQuery *query,
    const char *candidate);
#ifdef __cplusplus
}
#endif
#endif
