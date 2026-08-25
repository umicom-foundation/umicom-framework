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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_API_VERSION 1U
typedef enum UmiLanguageIntelligenceWorkspaceEditValidationDecision {
    UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_DENY = 0,
    UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_ALLOW = 1,
    UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_ALLOW_WITH_REFRESH = 2
} UmiLanguageIntelligenceWorkspaceEditValidationDecision;
typedef struct UmiLanguageIntelligenceWorkspaceEditValidationInput {
    UmiLanguageIntelligenceCapabilityFlags required_flags;
    UmiLanguageIntelligenceCapabilityFlags available_flags;
    uint32_t risk;
    uint32_t stale_age;
    uint32_t threshold;
    int hard_block;
    int explicit_override;
} UmiLanguageIntelligenceWorkspaceEditValidationInput;
UmiLanguageIntelligenceWorkspaceEditValidationDecision umi_language_intelligence_workspace_edit_validation_evaluate(
    const UmiLanguageIntelligenceWorkspaceEditValidationInput *input);
int umi_language_intelligence_workspace_edit_validation_capabilities_satisfied(
    const UmiLanguageIntelligenceWorkspaceEditValidationInput *input);
#ifdef __cplusplus
}
#endif
#endif
