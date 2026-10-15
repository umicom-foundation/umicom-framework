/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/ai/conversation_memory.c
 *
 * PURPOSE:
 *   Apply the shared AI Conversation Memory policy without replacing the canonical
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
#include "umicom/developer_intelligence/ai/conversation_memory.h"
#include "../developer_intelligence_internal.h"

UmiStatus UmiDeveloperIntelligenceEvaluateAiConversationMemory(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    static const UmiDeveloperIntelligencePolicy policy = {
        "ai.conversation-memory",
        "AI Conversation Memory",
        UMI_DEVELOPER_INTELLIGENCE_FRESHNESS,
        55.0,
        40.0,
        0.10,
        0.30,
        0.35,
        0.25,
        1U,
        1,
        1
    };
    return UmiDeveloperIntelligenceEvaluatePolicy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(UmiDeveloperIntelligenceEvaluateAiConversationMemoryBatch, UmiDeveloperIntelligenceEvaluateAiConversationMemory,
    UmiDeveloperIntelligenceInput, UmiDeveloperIntelligenceSnapshot)
