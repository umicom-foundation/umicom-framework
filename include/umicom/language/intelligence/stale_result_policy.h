/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/stale_result_policy.h
 *
 * PURPOSE:
 *   Decide whether stale provider results may be displayed, refreshed or rejected.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_H
#define UMICOM_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_API_VERSION 1U
/**
 * List the named language intelligence stale result policy decision values accepted by
 * this public contract.
 */
typedef enum UmiLanguageIntelligenceStaleResultPolicyDecision {
    UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_DENY = 0,
    UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_ALLOW = 1,
    UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_ALLOW_WITH_REFRESH = 2
} UmiLanguageIntelligenceStaleResultPolicyDecision;
/**
 * Represent the language intelligence stale result policy input data shared with callers
 * of this public contract.
 */
typedef struct UmiLanguageIntelligenceStaleResultPolicyInput {
    UmiLanguageIntelligenceCapabilityFlags required_flags;
    UmiLanguageIntelligenceCapabilityFlags available_flags;
    uint32_t risk;
    uint32_t stale_age;
    uint32_t threshold;
    int hard_block;
    int explicit_override;
} UmiLanguageIntelligenceStaleResultPolicyInput;
/**
 * Provide the language intelligence stale result policy evaluate operation used by this
 * module and its client applications.
 */
UmiLanguageIntelligenceStaleResultPolicyDecision umi_language_intelligence_stale_result_policy_evaluate(
    const UmiLanguageIntelligenceStaleResultPolicyInput *input);
/**
 * Provide the language intelligence stale result policy capabilities satisfied operation
 * used by this module and its client applications.
 */
int umi_language_intelligence_stale_result_policy_capabilities_satisfied(
    const UmiLanguageIntelligenceStaleResultPolicyInput *input);
#ifdef __cplusplus
}
#endif
#endif
