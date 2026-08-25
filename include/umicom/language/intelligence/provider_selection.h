/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/provider_selection.h
 *
 * PURPOSE:
 *   Rank provider candidates deterministically for a requested document and capability set.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_SELECTION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_SELECTION_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_PROVIDER_SELECTION_API_VERSION 1U
typedef struct UmiLanguageIntelligenceProviderSelectionInput {
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
} UmiLanguageIntelligenceProviderSelectionInput;
void umi_language_intelligence_provider_selection_input_init(UmiLanguageIntelligenceProviderSelectionInput *input);
uint64_t umi_language_intelligence_provider_selection_score(const UmiLanguageIntelligenceProviderSelectionInput *input);
int umi_language_intelligence_provider_selection_prefer(
    const UmiLanguageIntelligenceProviderSelectionInput *left,
    const UmiLanguageIntelligenceProviderSelectionInput *right);
#ifdef __cplusplus
}
#endif
#endif
