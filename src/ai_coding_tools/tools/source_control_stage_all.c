/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/source_control_stage_all.c
 *
 * PURPOSE:
 *   Stage all current changes through the provider-neutral controller.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/source_control_stage_all.h"
#include "../tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_source_control_stage_all_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "source-control.stage-all",
        .label = "Stage All Changes",
        .description = "Stage every current working-tree change.",
        .permission = "source-control.write",
        .risk = UMI_AI_CODING_TOOL_RISK_SOURCE_CONTROL_MUTATION,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_SOURCE_CONTROL_WRITE,
        .requires_approval = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_source_control_stage_all_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiStatus status;

    (void)arguments_json;

    if (environment == NULL || environment->source_control == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_developer_source_control_stage_all(
        environment->source_control);
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_coding_tool_json_status(
        output, output_capacity, UMI_STATUS_OK, "All changes staged.");
}
