/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/semantic_refresh.h
 *
 * PURPOSE:
 *   Track semantic-token refresh lifecycle and invalidation.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_SEMANTIC_REFRESH_H
#define UMICOM_LANGUAGE_INTELLIGENCE_SEMANTIC_REFRESH_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_REFRESH_API_VERSION 1U
/**
 * Represent the language intelligence semantic refresh data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceSemanticRefresh {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    UmiLanguageIntelligenceState state;
    uint32_t failure_count;
    uint32_t restart_count;
    uint64_t generation;
    uint64_t revision;
} UmiLanguageIntelligenceSemanticRefresh;
/**
 * Initialise language intelligence semantic refresh from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_semantic_refresh_init(UmiLanguageIntelligenceSemanticRefresh *value, const char *id);
/**
 * Provide the language intelligence semantic refresh transition operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_semantic_refresh_transition(
    UmiLanguageIntelligenceSemanticRefresh *value,
    UmiLanguageIntelligenceState next_state,
    int explicit_release);
/**
 * Provide the language intelligence semantic refresh record failure operation used by this
 * module and its client applications.
 */
void umi_language_intelligence_semantic_refresh_record_failure(UmiLanguageIntelligenceSemanticRefresh *value);
/**
 * Provide the language intelligence semantic refresh is usable operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_semantic_refresh_is_usable(const UmiLanguageIntelligenceSemanticRefresh *value);
#ifdef __cplusplus
}
#endif
#endif
