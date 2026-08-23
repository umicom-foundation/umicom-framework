/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/tool_test_support.c
 *
 * PURPOSE:
 *   Implement deterministic in-memory workspace, fake developer execution and
 *   scripted AI provider fixtures.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "tool_test_support.h"

#include <stdio.h>
#include <string.h>

UmiStatus tool_test_fixture_init(ToolTestFixture *fixture)
{
    UmiStatus status;

    if (fixture == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    (void)memset(fixture, 0, sizeof(*fixture));
    umi_ai_runtime_init(&fixture->runtime);
    test_workspace_init(&fixture->workspace_storage);

    status = test_workspace_adapter(
        &fixture->workspace_storage,
        &fixture->workspace);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_tool_environment_init(
        &fixture->environment,
        &fixture->runtime,
        NULL,
        &fixture->workspace,
        ".");
    if (status != UMI_STATUS_OK) return status;

    umi_ai_coding_checkpoint_store_init(&fixture->checkpoints);

    status = umi_ai_coding_tool_environment_set_checkpoints(
        &fixture->environment,
        &fixture->checkpoints);
    if (status != UMI_STATUS_OK) return status;

    fixture->runtime.policy.allow_tools = 1;
    fixture->runtime.policy.require_tool_approval = 1;

    status = umi_ai_coding_tool_register_all(&fixture->environment);
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_coding_tool_executor_init(
        &fixture->executor,
        &fixture->environment);
}

void tool_test_fixture_deinit(ToolTestFixture *fixture)
{
    if (fixture == NULL) return;
    umi_ai_coding_tool_executor_deinit(&fixture->executor);
}

UmiStatus tool_test_fake_execute(
    void *user_data,
    const UmiDeveloperOperationSnapshot *operation,
    UmiDeveloperExecutionResult *out_result)
{
    FakeExecutionState *state = (FakeExecutionState *)user_data;

    if (state == NULL || operation == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->struct_size = (uint32_t)sizeof(*out_result);
    out_result->api_version = UMI_DEVELOPER_EXECUTION_API_VERSION;
    out_result->exit_code = state->exit_code;
    out_result->launched = state->launched;
    out_result->duration_ms = 5U;

    if (state->output != NULL) {
        (void)snprintf(
            out_result->output,
            sizeof(out_result->output),
            "%s",
            state->output);
    }

    state->calls += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus fake_provider_generate(
    void *instance,
    const UmiAiRequest *request,
    UmiAiResponse *response)
{
    FakeProviderState *state = (FakeProviderState *)instance;
    const char *text;

    if (state == NULL || request == NULL || response == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (state->next_response >= state->response_count) {
        return UMI_STATUS_NOT_FOUND;
    }

    text = state->responses[state->next_response++];
    umi_ai_response_init(response);

    (void)snprintf(
        response->request_id,
        sizeof(response->request_id),
        "%s",
        request->request_id);
    (void)snprintf(
        response->provider_id,
        sizeof(response->provider_id),
        "%s",
        "test.provider");
    (void)snprintf(
        response->model_id,
        sizeof(response->model_id),
        "%s",
        request->model_id);
    response->finish_reason = UMI_AI_FINISH_STOP;

    return umi_ai_response_set_text(response, text);
}

UmiStatus tool_test_add_provider(
    UmiAiRuntime *runtime,
    const char *provider_id,
    FakeProviderState *state)
{
    UmiAiProvider provider;

    if (runtime == NULL || provider_id == NULL || state == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&provider, 0, sizeof(provider));
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.abi_version = 1U;
    provider.provider_id = provider_id;
    provider.kind = UMI_AI_PROVIDER_TEST;
    provider.instance = state;
    provider.generate = fake_provider_generate;

    return umi_ai_provider_registry_add(
        &runtime->providers,
        &provider);
}
