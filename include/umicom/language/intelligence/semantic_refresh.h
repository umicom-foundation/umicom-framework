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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_SEMANTIC_REFRESH_H
#define UMICOM_LANGUAGE_INTELLIGENCE_SEMANTIC_REFRESH_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_REFRESH_API_VERSION 1U
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
void umi_language_intelligence_semantic_refresh_init(UmiLanguageIntelligenceSemanticRefresh *value, const char *id);
UmiStatus umi_language_intelligence_semantic_refresh_transition(
    UmiLanguageIntelligenceSemanticRefresh *value,
    UmiLanguageIntelligenceState next_state,
    int explicit_release);
void umi_language_intelligence_semantic_refresh_record_failure(UmiLanguageIntelligenceSemanticRefresh *value);
int umi_language_intelligence_semantic_refresh_is_usable(const UmiLanguageIntelligenceSemanticRefresh *value);
#ifdef __cplusplus
}
#endif
#endif
