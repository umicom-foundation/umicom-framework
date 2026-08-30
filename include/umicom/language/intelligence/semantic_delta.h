/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/semantic_delta.h
 *
 * PURPOSE:
 *   Validate whether semantic-token delta application is safe.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_H
#define UMICOM_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_API_VERSION 1U
typedef enum UmiLanguageIntelligenceSemanticDeltaDecision {
    UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_DENY = 0,
    UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_ALLOW = 1,
    UMI_LANGUAGE_INTELLIGENCE_SEMANTIC_DELTA_ALLOW_WITH_REFRESH = 2
} UmiLanguageIntelligenceSemanticDeltaDecision;
typedef struct UmiLanguageIntelligenceSemanticDeltaInput {
    UmiLanguageIntelligenceCapabilityFlags required_flags;
    UmiLanguageIntelligenceCapabilityFlags available_flags;
    uint32_t risk;
    uint32_t stale_age;
    uint32_t threshold;
    int hard_block;
    int explicit_override;
} UmiLanguageIntelligenceSemanticDeltaInput;
UmiLanguageIntelligenceSemanticDeltaDecision umi_language_intelligence_semantic_delta_evaluate(
    const UmiLanguageIntelligenceSemanticDeltaInput *input);
int umi_language_intelligence_semantic_delta_capabilities_satisfied(
    const UmiLanguageIntelligenceSemanticDeltaInput *input);
#ifdef __cplusplus
}
#endif
#endif
