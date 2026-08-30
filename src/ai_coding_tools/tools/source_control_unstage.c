/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/source_control_unstage.c
 *
 * PURPOSE:
 *   Unstage one repository-relative path. Uses the provider-neutral source-control controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/source_control_unstage.h"
#include "../tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_source_control_unstage_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "source-control.unstage",
        .label = "Unstage Source File",
        .description = "Unstage one repository-relative path.",
        .permission = "source-control.write",
        .risk = UMI_AI_CODING_TOOL_RISK_SOURCE_CONTROL_MUTATION,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_SOURCE_CONTROL_WRITE,
        .requires_approval = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_source_control_unstage_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiLanguageRuntimeJsonDocument document;
    char path[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
    UmiStatus status;

    if (environment == NULL || environment->source_control == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_tool_json_parse_object(arguments_json, &document);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_safe_path(
            &document, "path", path, sizeof(path));
    }
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_source_control_unstage(
        environment->source_control, path);
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_coding_tool_json_status(
        output, output_capacity, UMI_STATUS_OK,
        "Unstage Source File completed.");
}
