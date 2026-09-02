/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/debug_stack.c
 *
 * PURPOSE:
 *   Refresh stack frames for a concrete DAP thread and return current frame
 *   counts/selection from the Framework debugger service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/debug_stack.h"
#include "../tool_support.h"

/*
 * Provide the ai coding tool debug stack descriptor operation used by this module and its
 * client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_debug_stack_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "debug.stack",
        .label = "Refresh Debug Stack",
        .description = "Refresh stack frames for one debugger thread.",
        .permission = "debug.read",
        .risk = UMI_AI_CODING_TOOL_RISK_READ_ONLY,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_DEBUG_READ,
        .requires_approval = 0,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

/*
 * Provide the ai coding tool debug stack invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_debug_stack_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiLanguageRuntimeJsonDocument document;
    UmiLanguageRuntimeJsonWriter writer;
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

    status = umi_debug_runtime_platform_refresh_stack(
        environment->debug_runtime,
        thread_id,
        (uint32_t)timeout);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_platform_snapshot(
        environment->debug_runtime, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(&writer, ",\"threadId\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, thread_id);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"frames\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.service.stack_frame_count);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"activeFrameId\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.active_frame_id);
    return umi_ai_coding_tool_write_status_end(&writer);
}
