/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/ai/workspace_memory.c
 *
 * PURPOSE:
 *   Apply the shared AI Workspace Memory policy without replacing the canonical
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
#include "umicom/developer_intelligence/ai/workspace_memory.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateAiWorkspaceMemory(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "ai.workspace-memory",
        "AI Workspace Memory",
        UMI_DEVELOPER_INTELLIGENCE_FRESHNESS,
        60.0,
        40.0,
        0.10,
        0.30,
        0.40,
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
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateAiWorkspaceMemoryBatch, UmiDeveloperIntelligenceEvaluateAiWorkspaceMemory,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
