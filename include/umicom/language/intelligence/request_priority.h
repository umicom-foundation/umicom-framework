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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_PRIORITY_H
#define UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_PRIORITY_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_REQUEST_PRIORITY_API_VERSION 1U
/**
 * Represent the language intelligence request priority input data shared with callers of
 * this public contract.
 */
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
/**
 * Initialise language intelligence request priority input from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_request_priority_input_init(UmiLanguageIntelligenceRequestPriorityInput *input);
/**
 * Provide the language intelligence request priority score operation used by this module
 * and its client applications.
 */
uint64_t umi_language_intelligence_request_priority_score(const UmiLanguageIntelligenceRequestPriorityInput *input);
/**
 * Provide the language intelligence request priority prefer operation used by this module
 * and its client applications.
 */
int umi_language_intelligence_request_priority_prefer(
    const UmiLanguageIntelligenceRequestPriorityInput *left,
    const UmiLanguageIntelligenceRequestPriorityInput *right);
#ifdef __cplusplus
}
#endif
#endif
