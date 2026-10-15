/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/ai/teacher_progress.c
 *
 * PURPOSE:
 *   Apply the shared AI Teacher Progress policy without replacing the canonical
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
#include "umicom/developer_intelligence/ai/teacher_progress.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateAiTeacherProgress(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "ai.teacher-progress",
        "AI Teacher Progress",
        UMI_DEVELOPER_INTELLIGENCE_MASTERY,
        65.0,
        30.0,
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
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateAiTeacherProgressBatch, UmiDeveloperIntelligenceEvaluateAiTeacherProgress,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
