/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/provider_supervision.h
 *
 * PURPOSE:
 *   Track persistent provider process supervision state and restart generation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_SUPERVISION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_SUPERVISION_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_PROVIDER_SUPERVISION_API_VERSION 1U
typedef struct UmiLanguageIntelligenceProviderSupervision {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    UmiLanguageIntelligenceState state;
    uint32_t failure_count;
    uint32_t restart_count;
    uint64_t generation;
    uint64_t revision;
} UmiLanguageIntelligenceProviderSupervision;
void umi_language_intelligence_provider_supervision_init(UmiLanguageIntelligenceProviderSupervision *value, const char *id);
UmiStatus umi_language_intelligence_provider_supervision_transition(
    UmiLanguageIntelligenceProviderSupervision *value,
    UmiLanguageIntelligenceState next_state,
    int explicit_release);
void umi_language_intelligence_provider_supervision_record_failure(UmiLanguageIntelligenceProviderSupervision *value);
int umi_language_intelligence_provider_supervision_is_usable(const UmiLanguageIntelligenceProviderSupervision *value);
#ifdef __cplusplus
}
#endif
#endif
