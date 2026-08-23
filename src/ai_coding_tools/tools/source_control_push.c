/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/source_control_push.c
 *
 * PURPOSE:
 *   Execute source-control.push only when network and source-control mutation policy are
 *   both explicitly enabled and the call is approved.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/source_control_push.h"
#include "../tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_source_control_push_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "source-control.push",
        .label = "Push Source Control",
        .description = "Push Source Control through the Framework source-control controller.",
        .permission = "source-control.network",
        .risk = UMI_AI_CODING_TOOL_RISK_NETWORK,
        .required_capabilities =
            UMI_AI_CODING_TOOL_CAP_SOURCE_CONTROL_WRITE |
            UMI_AI_CODING_TOOL_CAP_NETWORK,
        .requires_approval = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_source_control_push_invoke(
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

    status = umi_developer_source_control_push(
        environment->source_control);
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_coding_tool_json_status(
        output, output_capacity, UMI_STATUS_OK, "Push Source Control completed.");
}
