/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/workspace_exists.c
 *
 * PURPOSE:
 *   Check a normalized repository-relative workspace path.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/workspace_exists.h"
#include "../tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_workspace_exists_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "workspace.exists",
        .label = "Check Workspace Path",
        .description = "Check whether a repository-relative path exists.",
        .permission = "workspace.read",
        .risk = UMI_AI_CODING_TOOL_RISK_READ_ONLY,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_WORKSPACE_READ,
        .requires_approval = 0,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_workspace_exists_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiLanguageRuntimeJsonDocument document;
    UmiLanguageRuntimeJsonWriter writer;
    char path[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
    int exists = 0;
    UmiStatus status;

    if (environment == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_tool_json_parse_object(arguments_json, &document);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_safe_path(
            &document, "path", path, sizeof(path));
    }
    if (status == UMI_STATUS_OK) {
        status = environment->workspace.exists(
            environment->workspace.user_data,
            path,
            &exists);
    }
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(&writer, ",\"path\":");
    (void)umi_language_runtime_json_writer_string(&writer, path);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"exists\":");
    (void)umi_language_runtime_json_writer_bool(&writer, exists);
    return umi_ai_coding_tool_write_status_end(&writer);
}
