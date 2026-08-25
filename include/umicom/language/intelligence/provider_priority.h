/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/provider_priority.h
 *
 * PURPOSE:
 *   Calculate stable provider priority from explicit preference, project affinity and health.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_PRIORITY_H
#define UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_PRIORITY_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_PROVIDER_PRIORITY_API_VERSION 1U
typedef struct UmiLanguageIntelligenceProviderPriorityInput {
    uint32_t struct_size;
    uint32_t api_version;
    uint32_t base_priority;
    uint32_t exact_match;
    uint32_t project_affinity;
    uint32_t workspace_affinity;
    uint32_t health;
    uint32_t warm_state;
    uint32_t penalty;
    int enabled;
} UmiLanguageIntelligenceProviderPriorityInput;
void umi_language_intelligence_provider_priority_input_init(UmiLanguageIntelligenceProviderPriorityInput *input);
uint64_t umi_language_intelligence_provider_priority_score(const UmiLanguageIntelligenceProviderPriorityInput *input);
int umi_language_intelligence_provider_priority_prefer(
    const UmiLanguageIntelligenceProviderPriorityInput *left,
    const UmiLanguageIntelligenceProviderPriorityInput *right);
#ifdef __cplusplus
}
#endif
#endif
