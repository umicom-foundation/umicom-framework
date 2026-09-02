/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/agent_cancel.c
 *
 * PURPOSE:
 *   Request bounded coding-agent cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/agent_cancel.h"
#include "../tool_support.h"

/*
 * Provide the ai coding tool agent cancel descriptor operation used by this module and its
 * client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_agent_cancel_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "agent.cancel",
        .label = "Cancel Coding Agent",
        .description = "Request cancellation of the active coding task.",
        .permission = "agent.control",
        .risk = UMI_AI_CODING_TOOL_RISK_LOCAL_MUTATION,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_AGENT_CONTROL,
        .requires_approval = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

/*
 * Provide the ai coding tool agent cancel invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_agent_cancel_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;

    (void)arguments_json;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || environment->coding_runtime == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    umi_ai_coding_runtime_platform_cancel(environment->coding_runtime);

    return umi_ai_coding_tool_json_status(
        output, output_capacity, UMI_STATUS_OK, "Coding task cancellation requested.");
}
