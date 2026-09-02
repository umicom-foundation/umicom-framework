/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/provider_recovery.h
 *
 * PURPOSE:
 *   Decide bounded provider restart/recovery actions after failure.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_H
#define UMICOM_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_API_VERSION 1U
/**
 * List the named language intelligence provider recovery decision values accepted by this
 * public contract.
 */
typedef enum UmiLanguageIntelligenceProviderRecoveryDecision {
    UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_DENY = 0,
    UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_ALLOW = 1,
    UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_ALLOW_WITH_REFRESH = 2
} UmiLanguageIntelligenceProviderRecoveryDecision;
/**
 * Represent the language intelligence provider recovery input data shared with callers of
 * this public contract.
 */
typedef struct UmiLanguageIntelligenceProviderRecoveryInput {
    UmiLanguageIntelligenceCapabilityFlags required_flags;
    UmiLanguageIntelligenceCapabilityFlags available_flags;
    uint32_t risk;
    uint32_t stale_age;
    uint32_t threshold;
    int hard_block;
    int explicit_override;
} UmiLanguageIntelligenceProviderRecoveryInput;
/**
 * Provide the language intelligence provider recovery evaluate operation used by this
 * module and its client applications.
 */
UmiLanguageIntelligenceProviderRecoveryDecision umi_language_intelligence_provider_recovery_evaluate(
    const UmiLanguageIntelligenceProviderRecoveryInput *input);
/**
 * Provide the language intelligence provider recovery capabilities satisfied operation
 * used by this module and its client applications.
 */
int umi_language_intelligence_provider_recovery_capabilities_satisfied(
    const UmiLanguageIntelligenceProviderRecoveryInput *input);
#ifdef __cplusplus
}
#endif
#endif
