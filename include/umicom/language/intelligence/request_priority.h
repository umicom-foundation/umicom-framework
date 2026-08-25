/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/request_priority.h
 *
 * PURPOSE:
 *   Calculate deterministic request priority for interactive/background language work.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_PRIORITY_H
#define UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_PRIORITY_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_REQUEST_PRIORITY_API_VERSION 1U
typedef struct UmiLanguageIntelligenceRequestPriorityInput {
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
} UmiLanguageIntelligenceRequestPriorityInput;
void umi_language_intelligence_request_priority_input_init(UmiLanguageIntelligenceRequestPriorityInput *input);
uint64_t umi_language_intelligence_request_priority_score(const UmiLanguageIntelligenceRequestPriorityInput *input);
int umi_language_intelligence_request_priority_prefer(
    const UmiLanguageIntelligenceRequestPriorityInput *left,
    const UmiLanguageIntelligenceRequestPriorityInput *right);
#ifdef __cplusplus
}
#endif
#endif
