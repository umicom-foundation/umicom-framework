/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/agent_continue_repair.c
 *
 * PURPOSE:
 *   Continue the bounded repair loop after validation failure. Agent state mutation requires explicit approval.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/agent_continue_repair.h"
#include "../tool_support.h"

/*
 * Provide the ai coding tool agent continue repair descriptor operation used by this
 * module and its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_agent_continue_repair_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "agent.continue-repair",
        .label = "Continue Coding Repair",
        .description = "Continue the bounded repair loop after validation failure.",
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
 * Provide the ai coding tool agent continue repair invoke operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_coding_tool_agent_continue_repair_invoke(
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

    status = umi_ai_coding_runtime_platform_continue_repair(environment->coding_runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_coding_tool_json_status(
        output, output_capacity, UMI_STATUS_OK, "Continue Coding Repair completed.");
}
