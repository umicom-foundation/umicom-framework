/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/ai/teacher_recommendation.c
 *
 * PURPOSE:
 *   Apply the shared AI Teacher Recommendation policy without replacing the canonical
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
#include "umicom/developer_intelligence/ai/teacher_recommendation.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateAiTeacherRecommendation(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "ai.teacher-recommendation",
        "AI Teacher Recommendation",
        UMI_DEVELOPER_INTELLIGENCE_RETRIEVAL,
        60.0,
        35.0,
        0.15,
        0.30,
        0.35,
        0.20,
        1U,
        1,
        1
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateAiTeacherRecommendationBatch, UmiDeveloperIntelligenceEvaluateAiTeacherRecommendation,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
