/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/rename_preview.c
 *
 * PURPOSE:
 *   Implement summarise and validate rename-preview edit counts before mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/rename_preview.h"
/*
 * Provide the language intelligence rename preview capabilities satisfied operation used
 * by this module and its client applications.
 */
int umi_language_intelligence_rename_preview_capabilities_satisfied(
    const UmiLanguageIntelligenceRenamePreviewInput *input)
{
    return input != NULL &&
        (input->available_flags & input->required_flags) == input->required_flags;
}
/*
 * Provide the language intelligence rename preview evaluate operation used by this module
 * and its client applications.
 */
UmiLanguageIntelligenceRenamePreviewDecision umi_language_intelligence_rename_preview_evaluate(
    const UmiLanguageIntelligenceRenamePreviewInput *input)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || input->hard_block != 0)
        return UMI_LANGUAGE_INTELLIGENCE_RENAME_PREVIEW_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_language_intelligence_rename_preview_capabilities_satisfied(input) && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_RENAME_PREVIEW_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (input->risk > input->threshold && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_RENAME_PREVIEW_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (input->stale_age > input->threshold)
        return UMI_LANGUAGE_INTELLIGENCE_RENAME_PREVIEW_ALLOW_WITH_REFRESH;
    return UMI_LANGUAGE_INTELLIGENCE_RENAME_PREVIEW_ALLOW;
}
