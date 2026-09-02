/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/capability_negotiation.h
 *
 * PURPOSE:
 *   Negotiate required and optional language capabilities across provider boundaries.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_API_VERSION 1U
/**
 * List the named language intelligence capability negotiation decision values accepted by
 * this public contract.
 */
typedef enum UmiLanguageIntelligenceCapabilityNegotiationDecision {
    UMI_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_DENY = 0,
    UMI_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_ALLOW = 1,
    UMI_LANGUAGE_INTELLIGENCE_CAPABILITY_NEGOTIATION_ALLOW_WITH_REFRESH = 2
} UmiLanguageIntelligenceCapabilityNegotiationDecision;
/**
 * Represent the language intelligence capability negotiation input data shared with
 * callers of this public contract.
 */
typedef struct UmiLanguageIntelligenceCapabilityNegotiationInput {
    UmiLanguageIntelligenceCapabilityFlags required_flags;
    UmiLanguageIntelligenceCapabilityFlags available_flags;
    uint32_t risk;
    uint32_t stale_age;
    uint32_t threshold;
    int hard_block;
    int explicit_override;
} UmiLanguageIntelligenceCapabilityNegotiationInput;
/**
 * Provide the language intelligence capability negotiation evaluate operation used by this
 * module and its client applications.
 */
UmiLanguageIntelligenceCapabilityNegotiationDecision umi_language_intelligence_capability_negotiation_evaluate(
    const UmiLanguageIntelligenceCapabilityNegotiationInput *input);
/**
 * Provide the language intelligence capability negotiation capabilities satisfied
 * operation used by this module and its client applications.
 */
int umi_language_intelligence_capability_negotiation_capabilities_satisfied(
    const UmiLanguageIntelligenceCapabilityNegotiationInput *input);
#ifdef __cplusplus
}
#endif
#endif
