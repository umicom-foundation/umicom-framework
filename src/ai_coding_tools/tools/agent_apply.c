/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/agent_apply.c
 *
 * PURPOSE:
 *   Apply the approved governed coding patch. Agent state mutation requires explicit approval.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/agent_apply.h"
#include "../tool_support.h"

/*
 * Provide the ai coding tool agent apply descriptor operation used by this module and its
 * client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_agent_apply_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "agent.apply",
        .label = "Apply Pending Coding Patch",
        .description = "Apply the approved governed coding patch.",
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
 * Provide the ai coding tool agent apply invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_agent_apply_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiStatus status;

    (void)arguments_json;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || environment->coding_runtime == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_runtime_platform_apply(environment->coding_runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_coding_tool_json_status(
        output, output_capacity, UMI_STATUS_OK, "Apply Pending Coding Patch completed.");
}
