/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/provider_health.h
 *
 * PURPOSE:
 *   Track provider health and controlled transitions without application-owned state.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_HEALTH_H
#define UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_HEALTH_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_PROVIDER_HEALTH_API_VERSION 1U
typedef struct UmiLanguageIntelligenceProviderHealth {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    UmiLanguageIntelligenceState state;
    uint32_t failure_count;
    uint32_t restart_count;
    uint64_t generation;
    uint64_t revision;
} UmiLanguageIntelligenceProviderHealth;
void umi_language_intelligence_provider_health_init(UmiLanguageIntelligenceProviderHealth *value, const char *id);
UmiStatus umi_language_intelligence_provider_health_transition(
    UmiLanguageIntelligenceProviderHealth *value,
    UmiLanguageIntelligenceState next_state,
    int explicit_release);
void umi_language_intelligence_provider_health_record_failure(UmiLanguageIntelligenceProviderHealth *value);
int umi_language_intelligence_provider_health_is_usable(const UmiLanguageIntelligenceProviderHealth *value);
#ifdef __cplusplus
}
#endif
#endif
