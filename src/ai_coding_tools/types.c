/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/types.c
 *
 * PURPOSE:
 *   Implement stable diagnostic text for coding-tool risk and call states.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/types.h"

const char *umi_ai_coding_tool_risk_text(UmiAiCodingToolRisk risk)
{
    switch (risk) {
        case UMI_AI_CODING_TOOL_RISK_READ_ONLY: return "read-only";
        case UMI_AI_CODING_TOOL_RISK_LOCAL_MUTATION: return "local-mutation";
        case UMI_AI_CODING_TOOL_RISK_PROCESS_EXECUTION: return "process-execution";
        case UMI_AI_CODING_TOOL_RISK_SOURCE_CONTROL_MUTATION:
            return "source-control-mutation";
        case UMI_AI_CODING_TOOL_RISK_NETWORK: return "network";
        case UMI_AI_CODING_TOOL_RISK_DEBUG_CONTROL: return "debug-control";
        default: return "unknown";
    }
}

const char *umi_ai_coding_tool_call_state_text(UmiAiCodingToolCallState state)
{
    switch (state) {
        case UMI_AI_CODING_TOOL_CALL_PENDING: return "pending";
        case UMI_AI_CODING_TOOL_CALL_APPROVAL_REQUIRED:
            return "approval-required";
        case UMI_AI_CODING_TOOL_CALL_RUNNING: return "running";
        case UMI_AI_CODING_TOOL_CALL_SUCCEEDED: return "succeeded";
        case UMI_AI_CODING_TOOL_CALL_FAILED: return "failed";
        case UMI_AI_CODING_TOOL_CALL_REJECTED: return "rejected";
        case UMI_AI_CODING_TOOL_CALL_CANCELLED: return "cancelled";
        default: return "unknown";
    }
}
