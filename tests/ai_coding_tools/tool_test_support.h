/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/tool_test_support.h
 *
 * PURPOSE:
 *   Shared deterministic fixtures for coding-tool policy, registration,
 *   execution and provider/tool-loop tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_TEST_SUPPORT_H
#define UMICOM_AI_CODING_TOOLS_TEST_SUPPORT_H

#include "umicom/ai_coding_tools/ai_coding_tools.h"
#include "../ai_coding_runtime/workspace_test_support.h"

typedef struct ToolTestFixture {
    UmiAiRuntime runtime;
    TestWorkspace workspace_storage;
    UmiAiCodingWorkspaceAdapter workspace;
    UmiAiCodingToolEnvironment environment;
    UmiAiCodingCheckpointStore checkpoints;
    UmiAiCodingToolExecutor executor;
} ToolTestFixture;

UmiStatus tool_test_fixture_init(ToolTestFixture *fixture);
void tool_test_fixture_deinit(ToolTestFixture *fixture);

typedef struct FakeExecutionState {
    int exit_code;
    int launched;
    const char *output;
    size_t calls;
} FakeExecutionState;

UmiStatus tool_test_fake_execute(
    void *user_data,
    const UmiDeveloperOperationSnapshot *operation,
    UmiDeveloperExecutionResult *out_result);

typedef struct FakeProviderState {
    const char *responses[UMI_AI_CODING_TOOL_LOOP_MAX_TURNS + 1U];
    size_t response_count;
    size_t next_response;
} FakeProviderState;

UmiStatus tool_test_add_provider(
    UmiAiRuntime *runtime,
    const char *provider_id,
    FakeProviderState *state);

#endif
