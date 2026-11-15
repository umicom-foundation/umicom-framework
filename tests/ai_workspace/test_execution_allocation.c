/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_execution_allocation.c
 *
 * PURPOSE:
 *   Inject allocation failures around real reviewed execution without replacing workspace storage or runtime dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/ai_workspace/execution_internal.h"

static bool fail_calloc_once, fail_malloc_once;
static unsigned allocation_failures, provider_calls;
static void *ExecutionCalloc(size_t count, size_t size)
{
    if (fail_calloc_once) { fail_calloc_once = false; ++allocation_failures; return NULL; }
    return calloc(count, size);
}
static void *ExecutionMalloc(size_t size)
{
    if (fail_malloc_once) { fail_malloc_once = false; ++allocation_failures; return NULL; }
    return malloc(size);
}
/* Only allocation at the execution boundary is intercepted. Registry, policy,
 * Data Server transactions, response validation and storage recovery stay real. */
#define calloc ExecutionCalloc
#define malloc ExecutionMalloc
#include "../../src/ai_workspace/execution.c"
#undef malloc
#undef calloc

static UmiStatus Generate(void *context, const UmiAiRequest *request, UmiAiResponse *response)
{
    (void)context;
    ++provider_calls;
    umi_ai_response_init(response);
    memcpy(response->request_id, request->request_id, sizeof(response->request_id));
    memcpy(response->model_id, request->model_id, sizeof(response->model_id));
    strcpy(response->provider_id, "allocation.provider");
    response->finish_reason = UMI_AI_FINISH_STOP;
    return umi_ai_response_set_text(response, "A retained result.");
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    bool prepare_fault = strcmp(argv[1], "prepare") == 0;
    bool start_fault = strcmp(argv[1], "start") == 0;
    bool complete_fault = strcmp(argv[1], "complete") == 0;
    if (!prepare_fault && !start_fault && !complete_fault) return 2;
    UmiDataServer *data = NULL;
    UmiAiWorkspace *workspace = NULL;
    UmiAiRuntime runtime;
    umi_ai_runtime_init(&runtime);
    UmiAiProvider provider = {0};
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.abi_version = 1U;
    provider.provider_id = "allocation.provider";
    provider.kind = UMI_AI_PROVIDER_TEST;
    provider.generate = Generate;
    assert(umi_ai_provider_registry_add(&runtime.providers, &provider) == UMI_STATUS_OK);
    assert(umi_data_server_create_memory(&data) == UMI_STATUS_OK);
    assert(UmiAiWorkspaceCreate(data, &runtime, "allocation", &workspace) == UMI_STATUS_OK);
    assert(UmiAiWorkspacePrepare(workspace, "job.one", UMI_AI_WORKSPACE_DRAFT,
        "allocation.provider", "model", NULL, "Write a draft.", "author", 128U) == UMI_STATUS_OK);
    assert(UmiAiWorkspaceReview(workspace, "job.one", "reviewer", true) == UMI_STATUS_OK);
    uint64_t approved_revision = workspace->state->revision;
    AwExecution *execution = NULL;
    if (prepare_fault) {
        fail_calloc_once = true;
        assert(AwExecutionPrepare(workspace, "job.one", NULL, &execution) == UMI_STATUS_OUT_OF_MEMORY);
        assert(execution == NULL && workspace->execution_owners == 0U && !workspace->busy);
        assert(workspace->state->revision == approved_revision && provider_calls == 0U);
    }
    assert(AwExecutionPrepare(workspace, "job.one", NULL, &execution) == UMI_STATUS_OK);
    if (start_fault) {
        fail_malloc_once = true;
        assert(AwExecutionStart(execution) == UMI_STATUS_OUT_OF_MEMORY);
        assert(AwExecutionGetPhase(execution) == AW_EXECUTION_PREPARED);
        assert(workspace->state->revision == approved_revision && provider_calls == 0U);
    }
    assert(AwExecutionStart(execution) == UMI_STATUS_OK);
    assert(workspace->state->jobs[0].state == UMI_AI_WORKSPACE_RUNNING);
    assert(umi_ai_runtime_generate(&runtime, "allocation.provider", 1,
        AwExecutionRequest(execution), AwExecutionResponse(execution)) == UMI_STATUS_OK);
    if (complete_fault) fail_malloc_once = true;
    UmiStatus status = AwExecutionComplete(execution, UMI_STATUS_OK, false);
    assert(status == (complete_fault ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK));
    assert(AwExecutionOperationStatus(execution) == UMI_STATUS_OK);
    assert(AwExecutionPublicationStatus(execution) ==
        (complete_fault ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK));
    assert(AwExecutionGetPhase(execution) == AW_EXECUTION_SETTLED && !workspace->busy);
    assert(provider_calls == 1U && allocation_failures == 1U);
    assert(AwExecutionRelease(&execution) == UMI_STATUS_OK && execution == NULL);
    if (complete_fault) {
        assert(workspace->recoveryRequired);
        assert(UmiAiWorkspaceRun(workspace, "job.one", NULL) == UMI_STATUS_INVALID_STATE);
        assert(provider_calls == 1U);
        assert(UmiAiWorkspaceReload(workspace) == UMI_STATUS_OK);
        assert(workspace->state->jobs[0].state == UMI_AI_WORKSPACE_INTERRUPTED);
        assert(UmiAiWorkspaceRun(workspace, "job.one", NULL) == UMI_STATUS_INVALID_STATE);
        assert(provider_calls == 1U);
    } else {
        assert(workspace->state->jobs[0].state == UMI_AI_WORKSPACE_SUCCEEDED);
    }
    assert(UmiAiWorkspaceRelease(&workspace) == UMI_STATUS_OK && workspace == NULL);
    assert(UmiAiRuntimeRelease(&runtime) == UMI_STATUS_OK);
    umi_data_server_destroy(data);
    return 0;
}
