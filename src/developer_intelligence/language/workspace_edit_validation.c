/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/workspace_edit_validation.c
 *
 * PURPOSE:
 *   Apply the shared Workspace Edit Validation policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/workspace_edit_validation.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageWorkspaceEditValidation(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.workspace-edit-validation",
        "Workspace Edit Validation",
        UMI_DEVELOPER_INTELLIGENCE_APPROVAL,
        70.0,
        25.0,
        0.10,
        0.30,
        0.40,
        0.20,
        1U,
        1,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
