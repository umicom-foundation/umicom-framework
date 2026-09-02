/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/workspace_edit_validation.h
 *
 * PURPOSE:
 *   Validate workspace edit scope before any file mutation.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_API_VERSION 1U
/**
 * List the named language intelligence workspace edit validation decision values accepted
 * by this public contract.
 */
typedef enum UmiLanguageIntelligenceWorkspaceEditValidationDecision {
    UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_DENY = 0,
    UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_ALLOW = 1,
    UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_ALLOW_WITH_REFRESH = 2
} UmiLanguageIntelligenceWorkspaceEditValidationDecision;
/**
 * Represent the language intelligence workspace edit validation input data shared with
 * callers of this public contract.
 */
typedef struct UmiLanguageIntelligenceWorkspaceEditValidationInput {
    UmiLanguageIntelligenceCapabilityFlags required_flags;
    UmiLanguageIntelligenceCapabilityFlags available_flags;
    uint32_t risk;
    uint32_t stale_age;
    uint32_t threshold;
    int hard_block;
    int explicit_override;
} UmiLanguageIntelligenceWorkspaceEditValidationInput;
/**
 * Provide the language intelligence workspace edit validation evaluate operation used by
 * this module and its client applications.
 */
UmiLanguageIntelligenceWorkspaceEditValidationDecision umi_language_intelligence_workspace_edit_validation_evaluate(
    const UmiLanguageIntelligenceWorkspaceEditValidationInput *input);
/**
 * Provide the language intelligence workspace edit validation capabilities satisfied
 * operation used by this module and its client applications.
 */
int umi_language_intelligence_workspace_edit_validation_capabilities_satisfied(
    const UmiLanguageIntelligenceWorkspaceEditValidationInput *input);
#ifdef __cplusplus
}
#endif
#endif
