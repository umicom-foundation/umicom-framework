/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/language/project_template_score.c
 *
 * PURPOSE:
 *   Apply the shared Project Template Selection policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/language/project_template_score.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageProjectTemplateScore(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "language.project-template-score",
        "Project Template Selection",
        UMI_DEVELOPER_INTELLIGENCE_SCORE,
        60.0,
        45.0,
        0.25,
        0.20,
        0.35,
        0.20,
        0U,
        1,
        0
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}
