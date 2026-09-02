/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/request_coalescing.h
 *
 * PURPOSE:
 *   Detect equivalent requests that may be safely coalesced.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_H
#define UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_API_VERSION 1U
/**
 * List the named language intelligence request coalescing decision values accepted by this
 * public contract.
 */
typedef enum UmiLanguageIntelligenceRequestCoalescingDecision {
    UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_DENY = 0,
    UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_ALLOW = 1,
    UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_ALLOW_WITH_REFRESH = 2
} UmiLanguageIntelligenceRequestCoalescingDecision;
/**
 * Represent the language intelligence request coalescing input data shared with callers of
 * this public contract.
 */
typedef struct UmiLanguageIntelligenceRequestCoalescingInput {
    UmiLanguageIntelligenceCapabilityFlags required_flags;
    UmiLanguageIntelligenceCapabilityFlags available_flags;
    uint32_t risk;
    uint32_t stale_age;
    uint32_t threshold;
    int hard_block;
    int explicit_override;
} UmiLanguageIntelligenceRequestCoalescingInput;
/**
 * Provide the language intelligence request coalescing evaluate operation used by this
 * module and its client applications.
 */
UmiLanguageIntelligenceRequestCoalescingDecision umi_language_intelligence_request_coalescing_evaluate(
    const UmiLanguageIntelligenceRequestCoalescingInput *input);
/**
 * Provide the language intelligence request coalescing capabilities satisfied operation
 * used by this module and its client applications.
 */
int umi_language_intelligence_request_coalescing_capabilities_satisfied(
    const UmiLanguageIntelligenceRequestCoalescingInput *input);
#ifdef __cplusplus
}
#endif
#endif
