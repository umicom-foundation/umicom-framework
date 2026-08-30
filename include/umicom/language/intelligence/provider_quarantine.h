/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/provider_quarantine.h
 *
 * PURPOSE:
 *   Quarantine unstable providers and allow explicit controlled release.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_QUARANTINE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_QUARANTINE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_PROVIDER_QUARANTINE_API_VERSION 1U
typedef struct UmiLanguageIntelligenceProviderQuarantine {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    UmiLanguageIntelligenceState state;
    uint32_t failure_count;
    uint32_t restart_count;
    uint64_t generation;
    uint64_t revision;
} UmiLanguageIntelligenceProviderQuarantine;
void umi_language_intelligence_provider_quarantine_init(UmiLanguageIntelligenceProviderQuarantine *value, const char *id);
UmiStatus umi_language_intelligence_provider_quarantine_transition(
    UmiLanguageIntelligenceProviderQuarantine *value,
    UmiLanguageIntelligenceState next_state,
    int explicit_release);
void umi_language_intelligence_provider_quarantine_record_failure(UmiLanguageIntelligenceProviderQuarantine *value);
int umi_language_intelligence_provider_quarantine_is_usable(const UmiLanguageIntelligenceProviderQuarantine *value);
#ifdef __cplusplus
}
#endif
#endif
