/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/developer_configure.c
 *
 * PURPOSE:
 *   Execute a project configuration command. Execution is routed through the existing UmiDeveloperExecutor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/developer_configure.h"
#include "../process_tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_developer_configure_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "developer.configure",
        .label = "Configure Project",
        .description = "Execute a project configuration command.",
        .permission = "developer.execute",
        .risk = UMI_AI_CODING_TOOL_RISK_PROCESS_EXECUTION,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_PROCESS_EXECUTION,
        .requires_approval = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_developer_configure_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    return umi_ai_coding_tool_invoke_process_kind(
        (UmiAiCodingToolEnvironment *)user_data,
        UMI_DEVELOPER_OPERATION_CONFIGURE,
        "agent.configure",
        "Configure Project",
        arguments_json,
        output,
        output_capacity);
}
