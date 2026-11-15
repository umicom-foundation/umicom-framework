/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_owner_lifetime.c
 *
 * PURPOSE:
 *   Exercise guarded agent/platform execution with in-memory workspace, provider and validation callbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Include only the composition implementation to inject an in-memory owner.
 * Agent execution uses the linked Framework implementation and public APIs. */
#include "../../src/ai_coding_runtime/platform.c"
#include <stdio.h>
#define REQUIRE(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto cleanup; \
} } while (0)

typedef struct RuntimeFixture {
    UmiAiRuntime runtime;
    UmiAiAuthorEngineService *engine;
    UmiAiCodingRuntimePlatform *platform;
    UmiAiCodingRequest request;
    UmiAiCodingValidationPlan validation;
    UmiDeveloperExecutor executor;
    const char *phase, *fail_phase;
    int entered, failed, generates, writes, executes, fail_first_validation;
    int cancel_on_provider;
    char text[128];
} RuntimeFixture;

static void lifetime_hook(RuntimeFixture *f, const char *phase)
{
    if (f->phase == NULL || f->entered || strcmp(f->phase, phase) != 0) return;
    f->entered = 1;
    UmiAiCodingAgent *agent = f->platform->agent;
    UmiAiCodingAgentSnapshot before = {0}, after = {0};
    UmiAiCodingChangeGuardPolicy policy;
    UmiAiCodingValidationSelection selection;
    UmiAiCodingRuntimePlatform platform_before;
    memcpy(&platform_before, f->platform, sizeof platform_before);
    umi_ai_coding_change_guard_policy_init(&policy);
    if (umi_ai_coding_agent_snapshot(agent, &before) != UMI_STATUS_OK) f->failed = 1;
#define EXPECT_BUSY(operation) do { if ((operation) != UMI_STATUS_BUSY) f->failed = 1; } while (0)
    EXPECT_BUSY(UmiAiCodingAgentCheckIdle(agent));
    EXPECT_BUSY(UmiAiCodingRuntimePlatformCheckIdle(f->platform));
    EXPECT_BUSY(UmiAiCodingAgentDestroyChecked(agent));
    EXPECT_BUSY(UmiAiCodingRuntimePlatformDestroyChecked(f->platform));
    EXPECT_BUSY(umi_ai_coding_agent_run(agent, "nested", &f->request));
    EXPECT_BUSY(umi_ai_coding_agent_apply_pending(agent));
    EXPECT_BUSY(umi_ai_coding_agent_continue_repair(agent));
    EXPECT_BUSY(umi_ai_coding_agent_approve_pending(agent, "reviewer"));
    EXPECT_BUSY(umi_ai_coding_agent_reject_pending(agent));
    EXPECT_BUSY(umi_ai_coding_agent_set_validation_plan(agent, &f->validation));
    EXPECT_BUSY(umi_ai_coding_agent_set_change_guard(agent, &policy));
    EXPECT_BUSY(umi_ai_coding_runtime_platform_run(f->platform, "nested", &f->request));
    EXPECT_BUSY(umi_ai_coding_runtime_platform_run_next(f->platform));
    EXPECT_BUSY(umi_ai_coding_runtime_platform_enqueue(f->platform, "nested", &f->request));
    EXPECT_BUSY(umi_ai_coding_runtime_platform_apply(f->platform));
    EXPECT_BUSY(umi_ai_coding_runtime_platform_continue_repair(f->platform));
    EXPECT_BUSY(umi_ai_coding_runtime_platform_approve(f->platform, "reviewer"));
    EXPECT_BUSY(umi_ai_coding_runtime_platform_reject(f->platform));
    EXPECT_BUSY(umi_ai_coding_runtime_platform_refresh_repository(f->platform, UMI_AI_DATA_INTERNAL));
    EXPECT_BUSY(umi_ai_coding_runtime_platform_detect_validation(f->platform, &selection));
    EXPECT_BUSY(umi_ai_coding_runtime_platform_set_validation_plan(f->platform, &f->validation));
#undef EXPECT_BUSY
    umi_ai_coding_agent_destroy(agent);
    umi_ai_coding_runtime_platform_destroy(f->platform);
    if (umi_ai_coding_agent_snapshot(agent, &after) != UMI_STATUS_OK ||
        before.revision != after.revision || before.state != after.state ||
        memcmp(&platform_before, f->platform, sizeof platform_before) != 0 ||
        umi_ai_coding_runtime_platform_agent(f->platform) != agent)
        f->failed = 1;
}

static int fail_at(const RuntimeFixture *f, const char *phase)
{
    return f->fail_phase != NULL && strcmp(f->fail_phase, phase) == 0;
}

static UmiStatus runtime_read(void *user, const char *path, char *out,
    size_t capacity, size_t *length)
{
    RuntimeFixture *f = user;
    (void)path; lifetime_hook(f, "read");
    if (fail_at(f, "read")) return UMI_STATUS_IO_ERROR;
    *length = strlen(f->text);
    if (*length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, f->text, *length + 1U); return UMI_STATUS_OK;
}

static UmiStatus runtime_write(void *user, const char *path, const char *text, size_t length)
{
    RuntimeFixture *f = user;
    (void)path; ++f->writes; lifetime_hook(f, "write");
    if (fail_at(f, "write")) return UMI_STATUS_IO_ERROR;
    if (length >= sizeof f->text) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(f->text, text, length); f->text[length] = '\0'; return UMI_STATUS_OK;
}

static UmiStatus runtime_remove(void *user, const char *path)
{
    (void)user; (void)path; return UMI_STATUS_NOT_FOUND;
}

static UmiStatus runtime_exists(void *user, const char *path, int *exists)
{
    RuntimeFixture *f = user;
    if (f->phase != NULL && strcmp(f->phase, "exists") == 0 && !f->entered) {
        f->entered = 1;
        uint64_t revision = f->platform->revision;
        UmiAiCodingValidationSelection nested;
        if (UmiAiCodingRuntimePlatformCheckIdle(f->platform) != UMI_STATUS_BUSY ||
            UmiAiCodingRuntimePlatformDestroyChecked(f->platform) != UMI_STATUS_BUSY ||
            umi_ai_coding_runtime_platform_detect_validation(f->platform, &nested) != UMI_STATUS_BUSY ||
            umi_ai_coding_runtime_platform_run(f->platform, "nested", &f->request) != UMI_STATUS_BUSY)
            f->failed = 1;
        umi_ai_coding_runtime_platform_destroy(f->platform);
        if (f->platform->revision != revision) f->failed = 1;
    }
    *exists = strcmp(path, "src/example.c") == 0; return UMI_STATUS_OK;
}

static UmiStatus runtime_generate(void *user, const UmiAiRequest *request, UmiAiResponse *response)
{
    RuntimeFixture *f = user;
    (void)request; ++f->generates; lifetime_hook(f, "provider");
    if (f->cancel_on_provider) umi_ai_coding_runtime_platform_cancel(f->platform);
    if (fail_at(f, "provider")) return UMI_STATUS_IO_ERROR;
    return umi_ai_response_set_text(response,
        "UMICOM-CODING-RESPONSE/1\nSUMMARY-BEGIN\nUpdate value.\nSUMMARY-END\n"
        "PATCH-BEGIN\nTITLE|Update value\nRATIONALE-BEGIN\nReviewed example.\nRATIONALE-END\n"
        "FILE-BEGIN|MODIFY|src/example.c\nCONTENT-BEGIN\nint value = 2;\nCONTENT-END\n"
        "FILE-END\nPATCH-END\n");
}

static UmiStatus runtime_execute(void *user, const UmiDeveloperOperationSnapshot *operation,
    UmiDeveloperExecutionResult *out)
{
    RuntimeFixture *f = user;
    (void)operation; ++f->executes; lifetime_hook(f, "execute");
    memset(out, 0, sizeof *out);
    out->struct_size = sizeof *out; out->api_version = UMI_DEVELOPER_EXECUTION_API_VERSION;
    out->launched = 1;
    out->exit_code = fail_at(f, "execute") || (f->fail_first_validation && f->executes == 1);
    strcpy(out->output, out->exit_code ? "fixture validation failed" : "fixture validation passed");
    return UMI_STATUS_OK;
}

static UmiStatus runtime_fixture_init(RuntimeFixture *f, int automatic)
{
    UmiAiAuthorEngineConfig engine_config = {0};
    UmiAiRuntimeDescriptor descriptor = {0};
    UmiAiCodingRuntimeConfig config;
    UmiAiCodingContextFile context_file = {0};
    UmiAiCodingValidationStep step = {0};
    UmiAiCodingAgentBindings bindings = {0};
    UmiStatus status;
    umi_ai_runtime_init(&f->runtime);
    UmiAiProvider provider = {sizeof provider, 1U, "fixture.local", UMI_AI_PROVIDER_LOCAL,
        f, runtime_generate, NULL, NULL};
    status = umi_ai_provider_registry_add(&f->runtime.providers, &provider);
    if (status != UMI_STATUS_OK) return status;
    strcpy(engine_config.executable, "unused-fixture-engine");
    strcpy(engine_config.workspace, "."); strcpy(engine_config.provider, "fixture.local");
    status = umi_ai_authorengine_service_create(&f->runtime, &engine_config, &f->engine);
    if (status != UMI_STATUS_OK) return status;
    strcpy(descriptor.runtime_id, "runtime.fixture"); strcpy(descriptor.provider_id, "fixture.local");
    strcpy(descriptor.model_id, "fixture"); strcpy(descriptor.display_name, "Memory fixture");
    descriptor.provider_kind = UMI_AI_PROVIDER_LOCAL; descriptor.transport = UMI_AI_RUNTIME_PROCESS;
    descriptor.context_tokens = 8192U; descriptor.max_output_tokens = 1024U; descriptor.configured = 1;
    status = umi_ai_authorengine_service_register_runtime(f->engine, &descriptor);
    if (status != UMI_STATUS_OK) return status;
    f->platform = calloc(1U, sizeof *f->platform);
    if (f->platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    f->platform->ai_runtime = &f->runtime; f->platform->authorengine = f->engine;
    f->platform->revision = 1U; strcpy(f->platform->workspace_root, "memory-fixture");
    status = umi_ai_coding_assistant_create(f->engine, &f->platform->assistant);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_repository_index_create(
        umi_ai_coding_assistant_context(f->platform->assistant), &f->platform->repository_index);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_task_queue_create(&f->platform->task_queue);
    if (status != UMI_STATUS_OK) return status;
    strcpy(context_file.path, "src/example.c"); strcpy(context_file.language_id, "c");
    strcpy(context_file.summary, "Example value"); context_file.classification = UMI_AI_DATA_INTERNAL;
    context_file.enabled = 1; context_file.active = 1; context_file.estimated_tokens = 8U;
    /* Supply canonical ranking metadata so the lifetime assertions can be reached. */
    context_file.priority = 1U;
    status = umi_ai_coding_context_upsert(umi_ai_coding_assistant_context(f->platform->assistant), &context_file);
    if (status != UMI_STATUS_OK) return status;
    strcpy(f->text, "int value = 1;\n");
    f->platform->workspace = (UmiAiCodingWorkspaceAdapter){sizeof(UmiAiCodingWorkspaceAdapter),
        UMI_AI_CODING_RUNTIME_API_VERSION, runtime_read, runtime_write, runtime_remove, runtime_exists, f};
    umi_developer_executor_init(&f->executor, runtime_execute, f);
    f->platform->executor = &f->executor;
    bindings.ai_runtime = &f->runtime; bindings.assistant = f->platform->assistant;
    bindings.workspace = f->platform->workspace; bindings.executor = &f->executor;
    umi_ai_coding_runtime_config_init(&config);
    strcpy(config.provider_id, "fixture.local"); strcpy(config.model_id, "fixture");
    config.context_token_budget = 2048U; config.max_output_tokens = 512U; config.maximum_iterations = 2U;
    config.auto_approve = automatic; config.auto_apply_approved_patch = automatic;
    status = umi_ai_coding_agent_create(&bindings, &config, &f->platform->agent);
    if (status != UMI_STATUS_OK) return status;
    umi_ai_coding_request_init(&f->request, UMI_AI_CODING_TASK_REFACTOR);
    strcpy(f->request.request_id, "request.fixture"); strcpy(f->request.session_id, "session.fixture");
    strcpy(f->request.runtime_id, "runtime.fixture"); strcpy(f->request.workspace_root, "memory-fixture");
    strcpy(f->request.instruction, "Update the example value"); strcpy(f->request.active_path, "src/example.c");
    umi_ai_coding_validation_plan_init(&f->validation);
    strcpy(step.id, "fixture"); strcpy(step.label, "Memory validation"); strcpy(step.executable, "never-launched");
    step.kind = UMI_AI_CODING_VALIDATION_TEST; step.enabled = 1; step.required = 1;
    status = umi_ai_coding_validation_plan_add(&f->validation, &step);
    if (status != UMI_STATUS_OK) return status;
    return umi_ai_coding_runtime_platform_set_validation_plan(f->platform, &f->validation);
}

static void runtime_fixture_clear(RuntimeFixture *f)
{
    f->phase = NULL;
    if (UmiAiCodingRuntimePlatformDestroyChecked(f->platform) == UMI_STATUS_OK) f->platform = NULL;
    /* A refused platform must keep its borrowed engine alive for diagnosis. */
    if (f->platform == NULL) {
        umi_ai_authorengine_service_destroy(f->engine);
        (void)UmiAiRuntimeRelease(&f->runtime);
    }
}

static int execution_case(const char *entry, const char *phase)
{
    RuntimeFixture *f = calloc(1U, sizeof *f);
    UmiAiCodingAgentSnapshot snapshot;
    int result = 1;
    int manual = strcmp(entry, "manual") == 0 || strcmp(entry, "assistant") == 0;
    int repair = strcmp(entry, "repair") == 0;
    REQUIRE(f != NULL && runtime_fixture_init(f, !manual && !repair) == UMI_STATUS_OK);
    if (manual || repair) {
        REQUIRE(umi_ai_coding_runtime_platform_run(f->platform, "task.fixture", &f->request) == UMI_STATUS_OK);
        REQUIRE(umi_ai_coding_runtime_platform_approve(f->platform, "reviewer") == UMI_STATUS_OK);
        if (repair) {
            f->fail_first_validation = 1;
            REQUIRE(umi_ai_coding_runtime_platform_apply(f->platform) == UMI_STATUS_OK);
            REQUIRE(umi_ai_coding_agent_snapshot(f->platform->agent, &snapshot) == UMI_STATUS_OK);
            REQUIRE(snapshot.state == UMI_AI_CODING_RUNTIME_REPAIRING && snapshot.rolled_back);
        }
    }
    if (strcmp(entry, "queued") == 0)
        REQUIRE(umi_ai_coding_runtime_platform_enqueue(f->platform, "task.fixture", &f->request) == UMI_STATUS_OK);
    f->phase = phase;
    UmiStatus status;
    if (strcmp(entry, "direct") == 0)
        status = umi_ai_coding_agent_run(f->platform->agent, "task.fixture", &f->request);
    else if (strcmp(entry, "manual") == 0) status = umi_ai_coding_runtime_platform_apply(f->platform);
    else if (repair) status = umi_ai_coding_runtime_platform_continue_repair(f->platform);
    else if (strcmp(entry, "queued") == 0) status = umi_ai_coding_runtime_platform_run_next(f->platform);
    else if (strcmp(entry, "assistant") == 0) {
        UmiAiCodingFileAdapter adapter;
        REQUIRE(umi_ai_coding_agent_snapshot(f->platform->agent, &snapshot) == UMI_STATUS_OK);
        REQUIRE(umi_ai_coding_workspace_file_adapter(&f->platform->workspace, &adapter) == UMI_STATUS_OK);
        status = umi_ai_coding_assistant_apply_patch(f->platform->assistant, snapshot.patch_id, &adapter);
    } else status = umi_ai_coding_runtime_platform_run(f->platform, "task.fixture", &f->request);
    REQUIRE(status == UMI_STATUS_OK);
    if (repair) {
        /* Manual review remains required for the second proposal. */
        REQUIRE(umi_ai_coding_runtime_platform_approve(f->platform, "reviewer") == UMI_STATUS_OK);
        REQUIRE(umi_ai_coding_runtime_platform_apply(f->platform) == UMI_STATUS_OK);
    }
    REQUIRE(f->entered && !f->failed);
    REQUIRE(UmiAiCodingAgentCheckIdle(f->platform->agent) == UMI_STATUS_OK);
    REQUIRE(UmiAiCodingRuntimePlatformCheckIdle(f->platform) == UMI_STATUS_OK);
    REQUIRE(strcmp(f->text, "int value = 2;\n") == 0);
    REQUIRE(umi_ai_coding_agent_snapshot(f->platform->agent, &snapshot) == UMI_STATUS_OK);
    if (strcmp(entry, "assistant") != 0) REQUIRE(snapshot.state == UMI_AI_CODING_RUNTIME_COMPLETED);
    f->phase = NULL;
    REQUIRE(UmiAiCodingRuntimePlatformDestroyChecked(f->platform) == UMI_STATUS_OK);
    f->platform = NULL;
    result = 0;
cleanup:
    if (f != NULL) runtime_fixture_clear(f);
    free(f); return result;
}

static int failure_case(const char *phase)
{
    RuntimeFixture *f = calloc(1U, sizeof *f);
    int result = 1;
    REQUIRE(f != NULL && runtime_fixture_init(f, 1) == UMI_STATUS_OK);
    f->fail_phase = phase; f->phase = phase;
    REQUIRE(umi_ai_coding_runtime_platform_run(f->platform, "task.failure", &f->request) != UMI_STATUS_OK);
    REQUIRE(f->entered && !f->failed);
    REQUIRE(UmiAiCodingAgentCheckIdle(f->platform->agent) == UMI_STATUS_OK);
    REQUIRE(UmiAiCodingRuntimePlatformCheckIdle(f->platform) == UMI_STATUS_OK);
    REQUIRE(umi_ai_coding_runtime_platform_set_validation_plan(f->platform, &f->validation) == UMI_STATUS_OK);
    result = 0;
cleanup:
    if (f != NULL) runtime_fixture_clear(f);
    free(f); return result;
}

static int cancel_case(void)
{
    RuntimeFixture *f = calloc(1U, sizeof *f);
    int result = 1;
    REQUIRE(f != NULL && runtime_fixture_init(f, 1) == UMI_STATUS_OK);
    f->phase = "provider"; f->cancel_on_provider = 1;
    REQUIRE(umi_ai_coding_runtime_platform_run(f->platform, "task.cancel", &f->request) == UMI_STATUS_CANCELLED);
    REQUIRE(f->entered && !f->failed && f->writes == 0);
    REQUIRE(strcmp(f->text, "int value = 1;\n") == 0);
    REQUIRE(UmiAiCodingRuntimePlatformCheckIdle(f->platform) == UMI_STATUS_OK);
    result = 0;
cleanup:
    if (f != NULL) runtime_fixture_clear(f);
    free(f); return result;
}

static int selection_case(void)
{
    RuntimeFixture *f = calloc(1U, sizeof *f);
    UmiAiCodingValidationSelection selection;
    int result = 1;
    REQUIRE(f != NULL && runtime_fixture_init(f, 1) == UMI_STATUS_OK);
    f->phase = "exists";
    REQUIRE(umi_ai_coding_runtime_platform_detect_validation(f->platform, &selection) == UMI_STATUS_NOT_FOUND);
    REQUIRE(f->entered && !f->failed && !selection.detected);
    REQUIRE(UmiAiCodingRuntimePlatformCheckIdle(f->platform) == UMI_STATUS_OK);
    REQUIRE(umi_ai_coding_runtime_platform_set_validation_plan(f->platform, &f->validation) == UMI_STATUS_OK);
    result = 0;
cleanup:
    if (f != NULL) runtime_fixture_clear(f);
    free(f); return result;
}


int main(int argc, char **argv)
{
    if (argc == 4 && strcmp(argv[1], "execution") == 0) return execution_case(argv[2], argv[3]);
    if (argc == 3 && strcmp(argv[1], "failure") == 0) return failure_case(argv[2]);
    if (argc == 2 && strcmp(argv[1], "cancel") == 0) return cancel_case();
    if (argc == 2 && strcmp(argv[1], "selection") == 0) return selection_case();
    if (argc == 2 && strcmp(argv[1], "null") == 0)
        return UmiAiCodingAgentCheckIdle(NULL) != UMI_STATUS_OK ||
            UmiAiCodingAgentDestroyChecked(NULL) != UMI_STATUS_OK ||
            UmiAiCodingRuntimePlatformCheckIdle(NULL) != UMI_STATUS_OK ||
            UmiAiCodingRuntimePlatformDestroyChecked(NULL) != UMI_STATUS_OK;
    return 2;
}
