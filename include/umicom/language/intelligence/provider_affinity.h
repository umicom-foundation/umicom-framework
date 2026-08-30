/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/provider_affinity.h
 *
 * PURPOSE:
 *   Bind languages, workspaces or projects to preferred provider identities.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_AFFINITY_H
#define UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_AFFINITY_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_PROVIDER_AFFINITY_API_VERSION 1U

typedef struct UmiLanguageIntelligenceProviderAffinity {
    uint32_t struct_size;
    uint32_t api_version;
    char source_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char target_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char scope_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    uint32_t priority;
    int enabled;
    uint64_t revision;
} UmiLanguageIntelligenceProviderAffinity;

void umi_language_intelligence_provider_affinity_init(UmiLanguageIntelligenceProviderAffinity *mapping);
UmiStatus umi_language_intelligence_provider_affinity_set(
    UmiLanguageIntelligenceProviderAffinity *mapping,
    const char *source_id,
    const char *target_id,
    const char *scope_id);
UmiStatus umi_language_intelligence_provider_affinity_validate(
    const UmiLanguageIntelligenceProviderAffinity *mapping);
int umi_language_intelligence_provider_affinity_matches(
    const UmiLanguageIntelligenceProviderAffinity *mapping,
    const char *source_id,
    const char *scope_id);
#ifdef __cplusplus
}
#endif
#endif
