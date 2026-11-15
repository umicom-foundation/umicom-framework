/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_coding_task_admission.c
 *
 * PURPOSE:
 *   Verify canonical provider privacy, complete task publication and unchanged output/history on preparation refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* Include the real service only to inject exhausted/corrupt opaque ownership
 * fields. All normal behavior uses public APIs and offline runtime metadata. */
#include "../src/ai/coding_assistant.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto cleanup; \
} } while (0)
#define CASE(name) (strcmp(mode, (name)) == 0)

static void request_fill(UmiAiCodingRequest *request)
{
    umi_ai_coding_request_init(request, UMI_AI_CODING_TASK_EXPLAIN);
    strcpy(request->request_id, "request.example");
    strcpy(request->session_id, "session.example");
    strcpy(request->runtime_id, "runtime.example");
    strcpy(request->workspace_root, ".");
    strcpy(request->active_path, "src/example.c");
    strcpy(request->language_id, "c23");
    strcpy(request->instruction, "Explain the calculation");
    request->context_token_budget = 128U;
}

static int request_case(const char *mode)
{
    int result = 1;
    UmiAiCodingRequest request, before;
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    request_fill(&request);
    if (CASE("public")) { request.classification = UMI_AI_DATA_PUBLIC; expected = UMI_STATUS_OK; }
    else if (CASE("restricted")) { request.classification = UMI_AI_DATA_RESTRICTED; expected = UMI_STATUS_OK; }
    else if (CASE("class-low")) request.classification = (UmiAiDataClassification)0;
    else if (CASE("class-high")) request.classification = (UmiAiDataClassification)99;
    else if (CASE("instruction-bound")) memset(request.instruction, 'x', sizeof request.instruction);
    else if (CASE("runtime-bound")) memset(request.runtime_id, 'x', sizeof request.runtime_id);
    else if (CASE("selection")) { request.selection_start_line = 3U; request.selection_end_line = 2U; }
    else if (CASE("zero-budget")) request.context_token_budget = 0U;
    else if (CASE("file-limit")) request.maximum_context_files = UMI_AI_CODING_CONTEXT_PLAN_MAX + 1U;
    else if (CASE("unsafe-path")) { strcpy(request.active_path, "../outside"); expected = UMI_STATUS_PERMISSION_DENIED; }
    else if (CASE("null")) {
        REQUIRE(umi_ai_coding_request_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        result = 0; goto cleanup;
    } else { fprintf(stderr, "unknown request case: %s\n", mode); goto cleanup; }
    memcpy(&before, &request, sizeof before);
    REQUIRE(umi_ai_coding_request_validate(&request) == expected);
    REQUIRE(memcmp(&before, &request, sizeof before) == 0);
    result = 0;
cleanup:
    return result;
}

static int prepare_case(const char *mode)
{
    int result = 1;
    UmiAiRuntime runtime;
    UmiAiAuthorEngineConfig config = {0};
    UmiAiAuthorEngineService *engine = NULL;
    UmiAiCodingAssistantService *service = NULL, *before_service = NULL;
    UmiAiCodingTaskPlan *plan = malloc(sizeof *plan), *before_plan = malloc(sizeof *before_plan);
    UmiAiCodingTaskPlan *history = malloc(sizeof *history);
    UmiAiRuntimeDescriptor descriptor = {0};
    UmiAiCodingRequest request;
    UmiAiCodingContextFile file = {0};
    UmiAiPolicy provider = umi_ai_policy_default();
    UmiAiPrivacyPolicy privacy = umi_ai_privacy_policy_default();
    UmiAiProviderKind kind = UMI_AI_PROVIDER_LOCAL;
    UmiStatus status, expected = UMI_STATUS_OK;
    REQUIRE(plan != NULL && before_plan != NULL && history != NULL);
    umi_ai_runtime_init(&runtime);
    strcpy(config.executable, "uaengine"); strcpy(config.workspace, ".");
    strcpy(config.provider, "example.local");
    REQUIRE(umi_ai_authorengine_service_create(&runtime, &config, &engine) == UMI_STATUS_OK);
    strcpy(descriptor.runtime_id, "runtime.example"); strcpy(descriptor.provider_id, "example.local");
    strcpy(descriptor.display_name, "Offline test metadata"); strcpy(descriptor.model_id, "example");
    descriptor.provider_kind = UMI_AI_PROVIDER_LOCAL; descriptor.transport = UMI_AI_RUNTIME_PROCESS;
    descriptor.context_tokens = 2048U; descriptor.max_output_tokens = 128U; descriptor.configured = 1;
    if (CASE("remote-public") || CASE("remote-internal") || CASE("remote-as-local") ||
        CASE("remote-provider-denied") || CASE("remote-approval"))
        descriptor.provider_kind = UMI_AI_PROVIDER_REMOTE;
    if (CASE("authorengine")) descriptor.provider_kind = UMI_AI_PROVIDER_AUTHOR_ENGINE;
    if (CASE("test-provider")) descriptor.provider_kind = UMI_AI_PROVIDER_TEST;
    REQUIRE(umi_ai_authorengine_service_register_runtime(engine, &descriptor) == UMI_STATUS_OK);
    provider.allow_remote = 1; provider.require_remote_approval = 0;
    if (CASE("remote-provider-denied")) provider.allow_remote = 0;
    if (CASE("remote-approval")) provider.require_remote_approval = 1;
    privacy.maximum_local_classification = UMI_AI_DATA_RESTRICTED;
    privacy.maximum_remote_classification = UMI_AI_DATA_PUBLIC;
    REQUIRE(umi_ai_authorengine_service_set_policy(engine, &provider, &privacy, 2048U, 128U) == UMI_STATUS_OK);
    REQUIRE(umi_ai_coding_assistant_create(engine, &service) == UMI_STATUS_OK);
    strcpy(file.path, "src/example.c"); strcpy(file.language_id, "c23");
    file.classification = UMI_AI_DATA_INTERNAL; file.estimated_tokens = 64U;
    file.priority = 1U; file.enabled = 1;
    REQUIRE(umi_ai_coding_context_upsert(service->context, &file) == UMI_STATUS_OK);
    request_fill(&request);
    kind = descriptor.provider_kind;
    if (descriptor.provider_kind == UMI_AI_PROVIDER_REMOTE) request.classification = UMI_AI_DATA_PUBLIC;
    if (CASE("provider-low")) { kind = (UmiAiProviderKind)0; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (CASE("provider-high")) { kind = (UmiAiProviderKind)99; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (CASE("remote-as-local")) { kind = UMI_AI_PROVIDER_LOCAL; expected = UMI_STATUS_PERMISSION_DENIED; }
    else if (CASE("local-as-remote")) { kind = UMI_AI_PROVIDER_REMOTE; expected = UMI_STATUS_PERMISSION_DENIED; }
    else if (CASE("remote-internal")) { request.classification = UMI_AI_DATA_INTERNAL; expected = UMI_STATUS_PERMISSION_DENIED; }
    else if (CASE("remote-provider-denied") || CASE("remote-approval")) expected = UMI_STATUS_PERMISSION_DENIED;
    else if (CASE("sensitive-denied")) { request.classification = UMI_AI_DATA_CONFIDENTIAL; expected = UMI_STATUS_PERMISSION_DENIED; }
    else if (CASE("sensitive-approved")) { request.classification = UMI_AI_DATA_RESTRICTED; request.sensitive_approved = -1; }
    else if (CASE("class-low")) { request.classification = (UmiAiDataClassification)0; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (CASE("class-high")) { request.classification = (UmiAiDataClassification)99; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (CASE("runtime-missing")) { strcpy(request.runtime_id, "missing"); expected = UMI_STATUS_NOT_FOUND; }
    else if (CASE("owner-zero")) { service->revision = 0U; expected = UMI_STATUS_INVALID_STATE; }
    else if (CASE("owner-max")) { service->revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (CASE("owner-last")) service->revision = UINT64_MAX - 1U;
    else if (CASE("task-count")) { service->task_count = SIZE_MAX; expected = UMI_STATUS_INVALID_STATE; }
    else if (CASE("patch-count")) { service->patch_count = SIZE_MAX; expected = UMI_STATUS_INVALID_STATE; }
    else if (CASE("zero-repository-budget") || CASE("mandatory-general-denied")) {
        UmiAiContextSource general = {0};
        strcpy(general.source_id, "general"); strcpy(general.label, "General metadata");
        strcpy(general.uri, "workspace://root"); general.kind = UMI_AI_CONTEXT_WORKSPACE;
        general.classification = CASE("mandatory-general-denied") ? UMI_AI_DATA_RESTRICTED : UMI_AI_DATA_PUBLIC;
        general.estimated_tokens = 1920U; general.priority = 1U; general.enabled = general.mandatory = 1;
        REQUIRE(umi_ai_context_broker_upsert(umi_ai_authorengine_service_context(engine), &general) == UMI_STATUS_OK);
        if (CASE("mandatory-general-denied")) expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (CASE("history-full")) {
        for (size_t i = 0U; i < UMI_AI_CODING_TASK_HISTORY_CAPACITY; ++i) {
            snprintf(request.request_id, sizeof request.request_id, "request.%zu", i);
            REQUIRE(umi_ai_coding_assistant_prepare(service, &request, kind, plan) == UMI_STATUS_OK);
        }
        strcpy(request.request_id, "request.next");
    } else if (!CASE("valid") && !CASE("remote-public") && !CASE("authorengine") &&
        !CASE("test-provider") && !CASE("borrow-output") && !CASE("null-output") &&
        !CASE("null-request") && !CASE("null-service")) {
        fprintf(stderr, "unknown prepare case: %s\n", mode); goto cleanup;
    }
    memset(plan, 0x5a, sizeof *plan);
    if (CASE("borrow-output")) plan->request = request;
    memcpy(before_plan, plan, sizeof *plan);
    before_service = malloc(sizeof *service); REQUIRE(before_service != NULL);
    memcpy(before_service, service, sizeof *service);
    if (CASE("null-output")) {
        status = umi_ai_coding_assistant_prepare(service, &request, kind, NULL); expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (CASE("null-request")) {
        status = umi_ai_coding_assistant_prepare(service, NULL, kind, plan); expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (CASE("null-service")) {
        status = umi_ai_coding_assistant_prepare(NULL, &request, kind, plan); expected = UMI_STATUS_INVALID_ARGUMENT;
    } else status = umi_ai_coding_assistant_prepare(service, CASE("borrow-output") ? &plan->request : &request, kind, plan);
    REQUIRE(status == expected);
    if (status != UMI_STATUS_OK) {
        REQUIRE(memcmp(before_plan, plan, sizeof *plan) == 0);
        REQUIRE(memcmp(before_service, service, sizeof *service) == 0);
    } else {
        REQUIRE(plan->ready && plan->revision == before_service->revision + 1U);
        REQUIRE(service->revision == plan->revision);
        REQUIRE(strcmp(plan->request.instruction, "Explain the calculation") == 0);
        REQUIRE(umi_ai_coding_assistant_task_at(service, service->task_count - 1U, history) == UMI_STATUS_OK);
        REQUIRE(history->revision == plan->revision && history->plan_hash == plan->plan_hash);
        REQUIRE(history->repository_context.file_count == plan->repository_context.file_count);
        REQUIRE(strcmp(history->request.request_id, plan->request.request_id) == 0);
        if (CASE("remote-public") || CASE("zero-repository-budget")) {
            REQUIRE(plan->repository_context.file_count == 0U && plan->repository_context.dropped_count == 1U);
        } else REQUIRE(plan->repository_context.file_count == 1U);
        if (CASE("zero-repository-budget")) REQUIRE(plan->total_context_tokens == 1920U);
        if (CASE("history-full")) {
            REQUIRE(service->task_count == UMI_AI_CODING_TASK_HISTORY_CAPACITY);
            REQUIRE(strcmp(service->tasks[0].request.request_id, "request.1") == 0);
            REQUIRE(strcmp(service->tasks[service->task_count - 1U].request.request_id, "request.next") == 0);
        } else REQUIRE(service->task_count == 1U);
    }
    result = 0;
cleanup:
    free(before_service); free(history); free(before_plan); free(plan);
    umi_ai_coding_assistant_destroy(service); umi_ai_authorengine_service_destroy(engine);
    return result;
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (strcmp(argv[1], "request") == 0) return request_case(argv[2]);
    if (strcmp(argv[1], "prepare") == 0) return prepare_case(argv[2]);
    return 2;
}
