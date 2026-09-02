/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/agent_approve.c
 *
 * PURPOSE:
 *   Approve a pending coding patch with an explicit human/policy identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/agent_approve.h"
#include "../tool_support.h"

/*
 * Provide the ai coding tool agent approve descriptor operation used by this module and
 * its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_agent_approve_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "agent.approve",
        .label = "Approve Coding Patch",
        .description = "Approve the pending governed coding patch.",
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
 * Provide the ai coding tool agent approve invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_agent_approve_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiLanguageRuntimeJsonDocument document;
    char approved_by[UMI_AI_ID_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || environment->coding_runtime == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_tool_json_parse_object(arguments_json, &document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_required_string(
            &document, "approvedBy", approved_by, sizeof(approved_by));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_runtime_platform_approve(
        environment->coding_runtime,
        approved_by);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_coding_tool_json_status(
        output, output_capacity, UMI_STATUS_OK, "Coding patch approved.");
}
