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
/*
 * Provide the language intelligence workspace edit validation capabilities satisfied
 * operation used by this module and its client applications.
 */
int umi_language_intelligence_workspace_edit_validation_capabilities_satisfied(
    const UmiLanguageIntelligenceWorkspaceEditValidationInput *input)
{
    return input != NULL &&
        (input->available_flags & input->required_flags) == input->required_flags;
}
/*
 * Provide the language intelligence workspace edit validation evaluate operation used by
 * this module and its client applications.
 */
UmiLanguageIntelligenceWorkspaceEditValidationDecision umi_language_intelligence_workspace_edit_validation_evaluate(
    const UmiLanguageIntelligenceWorkspaceEditValidationInput *input)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || input->hard_block != 0)
        return UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_DENY;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_language_intelligence_workspace_edit_validation_capabilities_satisfied(input) && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (input->risk > input->threshold && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (input->stale_age > input->threshold)
        return UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_ALLOW_WITH_REFRESH;
    return UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_EDIT_VALIDATION_ALLOW;
}
