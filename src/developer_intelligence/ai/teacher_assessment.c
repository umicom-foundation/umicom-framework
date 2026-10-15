/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/ai/teacher_assessment.c
 *
 * PURPOSE:
 *   Apply the shared AI Teacher Assessment policy without replacing the canonical
 *   Language, AI, Knowledge, project or Teacher service that owns the data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/developer_intelligence/ai/teacher_assessment.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateAiTeacherAssessment(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "ai.teacher-assessment",
        "AI Teacher Assessment",
        UMI_DEVELOPER_INTELLIGENCE_MASTERY,
        70.0,
        25.0,
        0.10,
        0.25,
        0.35,
        0.30,
        1U,
        1,
        1
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateAiTeacherAssessmentBatch, UmiDeveloperIntelligenceEvaluateAiTeacherAssessment,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
