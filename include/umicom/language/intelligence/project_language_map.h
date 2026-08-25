/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/project_language_map.h
 *
 * PURPOSE:
 *   Bind a Framework project to its primary language profile.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_PROJECT_LANGUAGE_MAP_H
#define UMICOM_LANGUAGE_INTELLIGENCE_PROJECT_LANGUAGE_MAP_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_PROJECT_LANGUAGE_MAP_API_VERSION 1U

typedef struct UmiLanguageIntelligenceProjectLanguageMap {
    uint32_t struct_size;
    uint32_t api_version;
    char source_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char target_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char scope_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    uint32_t priority;
    int enabled;
    uint64_t revision;
} UmiLanguageIntelligenceProjectLanguageMap;

void umi_language_intelligence_project_language_map_init(UmiLanguageIntelligenceProjectLanguageMap *mapping);
UmiStatus umi_language_intelligence_project_language_map_set(
    UmiLanguageIntelligenceProjectLanguageMap *mapping,
    const char *source_id,
    const char *target_id,
    const char *scope_id);
UmiStatus umi_language_intelligence_project_language_map_validate(
    const UmiLanguageIntelligenceProjectLanguageMap *mapping);
int umi_language_intelligence_project_language_map_matches(
    const UmiLanguageIntelligenceProjectLanguageMap *mapping,
    const char *source_id,
    const char *scope_id);
#ifdef __cplusplus
}
#endif
#endif
