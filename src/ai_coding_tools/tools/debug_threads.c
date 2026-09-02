/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/debug_threads.c
 *
 * PURPOSE:
 *   Refresh the active DAP thread model and return current thread metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/debug_threads.h"
#include "../tool_support.h"

/*
 * Provide the ai coding tool debug threads descriptor operation used by this module and
 * its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_debug_threads_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "debug.threads",
        .label = "Refresh Debug Threads",
        .description = "Refresh the DAP thread model.",
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
 * Provide the ai coding tool debug threads invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_debug_threads_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiDebugRuntimePlatformSnapshot snapshot;
    UmiLanguageRuntimeJsonDocument document;
    UmiLanguageRuntimeJsonWriter writer;
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
            &document, "timeoutMs", 1000U, &timeout);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || timeout > UINT32_MAX) {
        return status != UMI_STATUS_OK
            ? status : UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_debug_runtime_platform_refresh_threads(
        environment->debug_runtime, (uint32_t)timeout);
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

    (void)umi_language_runtime_json_writer_raw(&writer, ",\"threads\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.service.thread_count);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"activeThreadId\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.active_thread_id);
    return umi_ai_coding_tool_write_status_end(&writer);
}
