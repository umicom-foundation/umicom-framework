/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/developer_test.c
 *
 * PURPOSE:
 *   Execute a project test command. Execution is routed through the existing UmiDeveloperExecutor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/developer_test.h"
#include "../process_tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_developer_test_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "developer.test",
        .label = "Run Project Tests",
        .description = "Execute a project test command.",
        .permission = "developer.execute",
        .risk = UMI_AI_CODING_TOOL_RISK_PROCESS_EXECUTION,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_PROCESS_EXECUTION,
        .requires_approval = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_developer_test_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    return umi_ai_coding_tool_invoke_process_kind(
        (UmiAiCodingToolEnvironment *)user_data,
        UMI_DEVELOPER_OPERATION_TEST,
        "agent.test",
        "Run Project Tests",
        arguments_json,
        output,
        output_capacity);
}
