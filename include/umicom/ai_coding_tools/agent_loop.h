/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/agent_loop.h
 *
 * PURPOSE:
 *   Run a bounded provider/tool/provider loop for repository-aware coding chat.
 *   Providers that lack native tool-calling can use the strict Umicom textual
 *   call/plan protocol while all execution still passes through central policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_AGENT_LOOP_H
#define UMICOM_AI_CODING_TOOLS_AGENT_LOOP_H

#include "umicom/ai_coding_tools/call_parser.h"
#include "umicom/ai_coding_tools/chat_session.h"
#include "umicom/ai_coding_tools/plan_executor.h"
#include "umicom/ai_coding_tools/plan_parser.h"
#include "umicom/ai_coding_tools/prompt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_CODING_TOOL_LOOP_MAX_TURNS 8U

typedef UmiStatus (*UmiAiCodingToolApprovalCallback)(
    void *user_data,
    const UmiAiCodingToolDescriptor *descriptor,
    const UmiAiCodingToolCall *call,
    int *out_approved);

typedef struct UmiAiCodingToolLoopConfig {
    uint32_t maximum_tool_turns;
    uint32_t max_output_tokens;
    double temperature;
    int provider_approved;
    UmiAiCodingToolApprovalCallback approval;
    void *approval_user_data;
} UmiAiCodingToolLoopConfig;

typedef struct UmiAiCodingToolLoopResult {
    UmiStatus status;
    uint32_t provider_turns;
    uint32_t tool_calls;
    uint32_t tool_plans;
    uint32_t approval_stops;
    char final_text[UMI_AI_CODING_TOOL_MAX_OUTPUT_BYTES];
    UmiAiCodingToolResult last_tool_result;
    UmiAiCodingToolPlanResult last_plan_result;
    uint64_t revision;
    int completed;
} UmiAiCodingToolLoopResult;

void umi_ai_coding_tool_loop_config_init(
    UmiAiCodingToolLoopConfig *config);

UmiStatus umi_ai_coding_tool_agent_loop_run(
    UmiAiRuntime *runtime,
    UmiAiCodingToolEnvironment *environment,
    UmiAiCodingToolExecutor *executor,
    UmiAiCodingToolChatSession *session,
    const UmiAiCodingToolLoopConfig *config,
    const char *user_message,
    UmiAiCodingToolLoopResult *out_result);

#ifdef __cplusplus
}
#endif
#endif
