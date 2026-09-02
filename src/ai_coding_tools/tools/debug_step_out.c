/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/debug_step_out.c
 *
 * PURPOSE:
 *   Step Out through the persistent DAP runtime. Debuggee control always requires
 *   explicit debug-control policy and approval.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/debug_step_out.h"
#include "../tool_support.h"

/*
 * Provide the ai coding tool debug step out descriptor operation used by this module and
 * its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_debug_step_out_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "debug.step-out",
        .label = "Step Out",
        .description = "Step Out through the Framework DAP runtime.",
        .permission = "debug.control",
        .risk = UMI_AI_CODING_TOOL_RISK_DEBUG_CONTROL,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_DEBUG_CONTROL,
        .requires_approval = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

/*
 * Provide the ai coding tool debug step out invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_debug_step_out_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiLanguageRuntimeJsonDocument document;
    UmiDebugRuntimePlatformSnapshot snapshot;
    uint64_t thread_id = 0U;
    uint64_t timeout = 1000U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || environment->debug_runtime == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_tool_json_parse_object(arguments_json, &document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_uint64(
            &document, "threadId", 0U, &thread_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_uint64(
            &document, "timeoutMs", 1000U, &timeout);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || timeout > UINT32_MAX) {
        return status != UMI_STATUS_OK
            ? status : UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (thread_id == 0U) {
        status = umi_debug_runtime_platform_snapshot(
            environment->debug_runtime, &snapshot);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        thread_id = snapshot.active_thread_id;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (thread_id == 0U) return UMI_STATUS_NOT_FOUND;

    status = umi_debug_runtime_platform_step_out(
        environment->debug_runtime,
        thread_id,
        (uint32_t)timeout);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_coding_tool_json_status(
        output, output_capacity, UMI_STATUS_OK, "Step Out completed.");
}
