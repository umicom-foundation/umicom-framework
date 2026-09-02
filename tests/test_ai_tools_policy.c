/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_tools_policy.c
 *
 * PURPOSE:
 *   Validate the AI and Helix foundation introduced by Batch 10.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused executable keeps one contract easy to diagnose when the larger test suite reports a failure.
 */



#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/umicom.h"

/*
 * Exercise tool call and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus tool_call(const char *args, char *out, size_t cap, void *data)
{
    int written;
    (void)data;
    written = snprintf(out, cap, "%s", args);
    return written >= 0 && (size_t)written < cap ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiRuntime runtime;
    UmiAiTool tool;
    char output[64];
    umi_ai_runtime_init(&runtime);
    runtime.policy.allow_tools = 1;
    assert(umi_ai_tool_init(&tool, "echo", "echo", "test.echo", tool_call, NULL) == UMI_STATUS_OK);
    assert(umi_ai_tool_registry_add(&runtime.tools, &tool) == UMI_STATUS_OK);
    assert(umi_ai_runtime_invoke_tool(&runtime, "echo", 0, "x", output, sizeof(output))
           == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_ai_runtime_invoke_tool(&runtime, "echo", 1, "x", output, sizeof(output))
           == UMI_STATUS_OK);
    assert(strcmp(output, "x") == 0);
    return 0;
}
