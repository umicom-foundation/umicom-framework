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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_SUPERVISION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_SUPERVISION_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_PROVIDER_SUPERVISION_API_VERSION 1U
/**
 * Represent the language intelligence provider supervision data shared with callers of
 * this public contract.
 */
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
/**
 * Initialise language intelligence provider supervision from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_provider_supervision_init(UmiLanguageIntelligenceProviderSupervision *value, const char *id);
/**
 * Provide the language intelligence provider supervision transition operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_provider_supervision_transition(
    UmiLanguageIntelligenceProviderSupervision *value,
    UmiLanguageIntelligenceState next_state,
    int explicit_release);
/**
 * Provide the language intelligence provider supervision record failure operation used by
 * this module and its client applications.
 */
void umi_language_intelligence_provider_supervision_record_failure(UmiLanguageIntelligenceProviderSupervision *value);
/**
 * Provide the language intelligence provider supervision is usable operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_provider_supervision_is_usable(const UmiLanguageIntelligenceProviderSupervision *value);
#ifdef __cplusplus
}
#endif
#endif
