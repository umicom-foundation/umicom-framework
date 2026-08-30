/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/source_control_diff.c
 *
 * PURPOSE:
 *   Return a source-control diff through the existing controller rather than
 *   allowing a model to invoke Git directly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/source_control_diff.h"
#include "../tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_source_control_diff_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "source-control.diff",
        .label = "Source Control Diff",
        .description = "Read a working-tree or staged diff.",
        .permission = "source-control.read",
        .risk = UMI_AI_CODING_TOOL_RISK_READ_ONLY,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_SOURCE_CONTROL_READ,
        .requires_approval = 0,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_source_control_diff_invoke(
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
    char diff[UMI_AI_CODING_TOOL_MAX_OUTPUT_BYTES / 2U];
    int staged = 0;
    UmiStatus status;

    if (environment == NULL || environment->source_control == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_tool_json_parse_object(arguments_json, &document);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_safe_path(
            &document, "path", path, sizeof(path));
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_bool(
            &document, "staged", 0, &staged);
    }
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_source_control_diff(
        environment->source_control,
        path,
        staged,
        diff,
        sizeof(diff));
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(&writer, ",\"staged\":");
    (void)umi_language_runtime_json_writer_bool(&writer, staged);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"diff\":");
    (void)umi_language_runtime_json_writer_string(&writer, diff);
    return umi_ai_coding_tool_write_status_end(&writer);
}
