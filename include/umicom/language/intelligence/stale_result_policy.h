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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_H
#define UMICOM_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_API_VERSION 1U
typedef enum UmiLanguageIntelligenceStaleResultPolicyDecision {
    UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_DENY = 0,
    UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_ALLOW = 1,
    UMI_LANGUAGE_INTELLIGENCE_STALE_RESULT_POLICY_ALLOW_WITH_REFRESH = 2
} UmiLanguageIntelligenceStaleResultPolicyDecision;
typedef struct UmiLanguageIntelligenceStaleResultPolicyInput {
    UmiLanguageIntelligenceCapabilityFlags required_flags;
    UmiLanguageIntelligenceCapabilityFlags available_flags;
    uint32_t risk;
    uint32_t stale_age;
    uint32_t threshold;
    int hard_block;
    int explicit_override;
} UmiLanguageIntelligenceStaleResultPolicyInput;
UmiLanguageIntelligenceStaleResultPolicyDecision umi_language_intelligence_stale_result_policy_evaluate(
    const UmiLanguageIntelligenceStaleResultPolicyInput *input);
int umi_language_intelligence_stale_result_policy_capabilities_satisfied(
    const UmiLanguageIntelligenceStaleResultPolicyInput *input);
#ifdef __cplusplus
}
#endif
#endif
