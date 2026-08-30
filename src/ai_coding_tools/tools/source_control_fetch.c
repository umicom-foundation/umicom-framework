/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/source_control_fetch.c
 *
 * PURPOSE:
 *   Execute source-control.fetch only when network and source-control mutation policy are
 *   both explicitly enabled and the call is approved.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/source_control_fetch.h"
#include "../tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_source_control_fetch_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "source-control.fetch",
        .label = "Fetch Source Control",
        .description = "Fetch Source Control through the Framework source-control controller.",
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

UmiStatus umi_ai_coding_tool_source_control_fetch_invoke(
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

    status = umi_developer_source_control_fetch(
        environment->source_control);
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_coding_tool_json_status(
        output, output_capacity, UMI_STATUS_OK, "Fetch Source Control completed.");
}
