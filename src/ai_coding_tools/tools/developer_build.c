/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/developer_build.c
 *
 * PURPOSE:
 *   Execute a project build command. Execution is routed through the existing UmiDeveloperExecutor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/developer_build.h"
#include "../process_tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_developer_build_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "developer.build",
        .label = "Build Project",
        .description = "Execute a project build command.",
        .permission = "developer.execute",
        .risk = UMI_AI_CODING_TOOL_RISK_PROCESS_EXECUTION,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_PROCESS_EXECUTION,
        .requires_approval = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_developer_build_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    return umi_ai_coding_tool_invoke_process_kind(
        (UmiAiCodingToolEnvironment *)user_data,
        UMI_DEVELOPER_OPERATION_BUILD,
        "agent.build",
        "Build Project",
        arguments_json,
        output,
        output_capacity);
}
