/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/workspace_edit_validation.c
 *
 * PURPOSE:
 *   Implement validate workspace edit scope before any file mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/workspace_edit_validation.h"
int umi_language_intelligence_workspace_edit_validation_capabilities_satisfied(
    const UmiLanguageIntelligenceWorkspaceEditValidationInput *input)
{
    return input != NULL &&
        (input->available_flags & input->required_flags) == input->required_flags;
}
UmiLanguageIntelligenceWorkspaceEditValidationDecision umi_language_intelligence_workspace_edit_validation_evaluate(
    const UmiLanguageIntelligenceWorkspaceEditValidationInput *input)
{
    if (input == NULL || input->hard_block != 0)
        return UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_DENY;
    if (!umi_language_intelligence_workspace_edit_validation_capabilities_satisfied(input) && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_DENY;
    if (input->risk > input->threshold && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_DENY;
    if (input->stale_age > input->threshold)
        return UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_ALLOW_WITH_REFRESH;
    return UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_ALLOW;
}
