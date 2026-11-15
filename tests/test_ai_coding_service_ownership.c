/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_coding_service_ownership.c
 *
 * PURPOSE:
 *   Verify service revision admission, bounded owner reads and callback mutation/destruction guards with memory adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* Include the implementation solely for opaque owner boundary injection.
 * Exercise normal behavior through public APIs and memory-only file adapters. */
#include "../src/ai/coding_assistant.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto cleanup; \
} } while (0)

typedef struct OwnershipFixture {
    UmiAiRuntime runtime;
    UmiAiAuthorEngineService *engine;
    UmiAiCodingAssistantService *service;
    UmiAiCodingPatch *draft, *read_patch;
    UmiAiCodingTaskPlan *plan;
    UmiAiCodingRequest request;
    char text[64];
    int reads, writes, removes, fail_first_write;
    int fail_compensation_write, fail_restoration_read;
    int entered, hook_failed;
    const char *hook, *phase;
    UmiAiCodingFileAdapter *original_adapter;
} OwnershipFixture;

static UmiStatus nested_operation(OwnershipFixture *f)
{
    UmiAiCodingPatchPolicy policy = umi_ai_coding_patch_policy_default();
    if (strcmp(f->hook, "policy") == 0)
        return umi_ai_coding_assistant_set_patch_policy(f->service, &policy);
    if (strcmp(f->hook, "prepare") == 0)
        return umi_ai_coding_assistant_prepare(f->service, &f->request, UMI_AI_PROVIDER_LOCAL, f->plan);
    if (strcmp(f->hook, "record") == 0)
        return umi_ai_coding_assistant_record_patch(f->service, f->draft);
    if (strcmp(f->hook, "approve") == 0)
        return umi_ai_coding_assistant_approve_patch(f->service, "patch.example", "reviewer");
    if (strcmp(f->hook, "reject") == 0)
        return umi_ai_coding_assistant_reject_patch(f->service, "patch.example");
    if (strcmp(f->hook, "apply") == 0)
        return umi_ai_coding_assistant_apply_patch(f->service, "patch.example", f->original_adapter);
    if (strcmp(f->hook, "revert") == 0)
        return umi_ai_coding_assistant_revert_patch(f->service, "patch.example", f->original_adapter);
    if (strcmp(f->hook, "apply-report") == 0 || strcmp(f->hook, "revert-report") == 0) {
        UmiAiCodingPatchTransactionReport report = {0};
        UmiStatus status = strcmp(f->hook, "apply-report") == 0 ?
            UmiAiCodingAssistantApplyPatchWithReport(f->service, "patch.example", f->original_adapter, &report) :
            UmiAiCodingAssistantRevertPatchWithReport(f->service, "patch.example", f->original_adapter, &report);
        return report.admitted ? UMI_STATUS_INTERNAL_ERROR : status;
    }
    if (strcmp(f->hook, "destroy") == 0)
        return UmiAiCodingAssistantDestroyChecked(f->service);
    return UMI_STATUS_INTERNAL_ERROR;
}

static void callback_hook(OwnershipFixture *f, const char *phase)
{
    if (f->hook == NULL || f->entered || strcmp(f->phase, phase) != 0) return;
    f->entered = 1;
    uint64_t revision = f->service->revision;
    if (strcmp(f->hook, "legacy-destroy") == 0) {
        umi_ai_coding_assistant_destroy(f->service);
        if (!f->service->patch_transaction_active) f->hook_failed = 1;
    } else if (strcmp(f->hook, "reads") == 0) {
        UmiAiCodingAssistantSnapshot snapshot;
        if (umi_ai_coding_assistant_snapshot(f->service, &snapshot) != UMI_STATUS_OK ||
            snapshot.revision != revision ||
            umi_ai_coding_assistant_find_patch(f->service, "patch.example", f->read_patch) != UMI_STATUS_OK ||
            umi_ai_coding_assistant_task_at(f->service, 0U, f->plan) != UMI_STATUS_OK)
            f->hook_failed = 1;
    } else if (strcmp(f->hook, "adapter-copy") == 0) {
        /* The outer operation must keep using the admitted callback slots. */
        f->original_adapter->write = NULL;
        f->original_adapter->remove = NULL;
    } else if (nested_operation(f) != UMI_STATUS_BUSY) f->hook_failed = 1;
    if (f->service->revision != revision || !f->service->patch_transaction_active)
        f->hook_failed = 1;
}

static UmiStatus fixture_read(void *user, const char *path, char *out,
    size_t capacity, size_t *length)
{
    OwnershipFixture *f = user;
    (void)path; ++f->reads; callback_hook(f, "read");
    if (f->fail_restoration_read && f->reads > 1) return UMI_STATUS_IO_ERROR;
    *length = strlen(f->text);
    if (*length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, f->text, *length);
    return UMI_STATUS_OK;
}

static UmiStatus fixture_write(void *user, const char *path, const char *text, size_t length)
{
    OwnershipFixture *f = user;
    (void)path; ++f->writes;
    callback_hook(f, f->writes == 1 ? "write" : "compensation");
    if (f->fail_first_write && f->writes == 1) return UMI_STATUS_IO_ERROR;
    if (f->fail_compensation_write && f->writes == 2) return UMI_STATUS_PERMISSION_DENIED;
    if (length >= sizeof f->text) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(f->text, text, length); f->text[length] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus fixture_remove(void *user, const char *path)
{
    OwnershipFixture *f = user;
    (void)path; ++f->removes;
    return UMI_STATUS_NOT_FOUND;
}

static UmiAiCodingFileAdapter fixture_adapter(OwnershipFixture *f)
{
    return (UmiAiCodingFileAdapter){sizeof(UmiAiCodingFileAdapter), UMI_AI_CODING_ABI_VERSION,
        fixture_read, fixture_write, fixture_remove, f};
}

static UmiStatus fixture_init(OwnershipFixture *f)
{
    UmiAiAuthorEngineConfig config = {0};
    UmiAiRuntimeDescriptor runtime = {0};
    UmiStatus status;
    memset(f, 0, sizeof *f);
    f->draft = malloc(sizeof *f->draft); f->read_patch = malloc(sizeof *f->read_patch);
    f->plan = malloc(sizeof *f->plan);
    if (f->draft == NULL || f->read_patch == NULL || f->plan == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_ai_runtime_init(&f->runtime);
    strcpy(config.executable, "uaengine"); strcpy(config.workspace, "."); strcpy(config.provider, "example.local");
    status = umi_ai_authorengine_service_create(&f->runtime, &config, &f->engine);
    if (status != UMI_STATUS_OK) return status;
    strcpy(runtime.runtime_id, "runtime.example"); strcpy(runtime.provider_id, "example.local");
    strcpy(runtime.display_name, "Offline owner test"); strcpy(runtime.model_id, "example");
    runtime.provider_kind = UMI_AI_PROVIDER_LOCAL; runtime.transport = UMI_AI_RUNTIME_PROCESS;
    runtime.context_tokens = 2048U; runtime.max_output_tokens = 128U; runtime.configured = 1;
    status = umi_ai_authorengine_service_register_runtime(f->engine, &runtime);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_assistant_create(f->engine, &f->service);
    if (status != UMI_STATUS_OK) return status;
    umi_ai_coding_request_init(&f->request, UMI_AI_CODING_TASK_EXPLAIN);
    strcpy(f->request.request_id, "request.example"); strcpy(f->request.session_id, "session.example");
    strcpy(f->request.runtime_id, "runtime.example"); strcpy(f->request.workspace_root, ".");
    strcpy(f->request.instruction, "Explain this example");
    status = umi_ai_coding_assistant_prepare(f->service, &f->request, UMI_AI_PROVIDER_LOCAL, f->plan);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_patch_init(f->draft, "patch.example", "request.example", "Update example", "Reviewed update");
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_patch_add_file(f->draft, "src/example.c", UMI_AI_CODING_PATCH_MODIFY, "before", "after");
    if (status != UMI_STATUS_OK) return status;
    strcpy(f->text, "before");
    return umi_ai_coding_assistant_record_patch(f->service, f->draft);
}

static void fixture_clear(OwnershipFixture *f)
{
    umi_ai_coding_assistant_destroy(f->service);
    umi_ai_authorengine_service_destroy(f->engine);
    free(f->draft); free(f->read_patch); free(f->plan);
}

static UmiStatus mutate(OwnershipFixture *f, const char *operation, UmiAiCodingFileAdapter *adapter)
{
    /* Route the requested operation through the common dispatcher without
     * enabling a callback hook in the ordinary revision-boundary fixtures. */
    f->hook = operation; f->phase = "disabled"; f->original_adapter = adapter;
    UmiStatus status = nested_operation(f);
    f->hook = NULL; return status;
}

static int mutation_case(const char *operation, int final_revision)
{
    OwnershipFixture f = {0};
    UmiAiCodingAssistantService *before = NULL;
    int result = 1;
    REQUIRE(fixture_init(&f) == UMI_STATUS_OK);
    UmiAiCodingFileAdapter adapter = fixture_adapter(&f);
    if (strcmp(operation, "apply") == 0 || strcmp(operation, "revert") == 0)
        REQUIRE(umi_ai_coding_assistant_approve_patch(f.service, "patch.example", "reviewer") == UMI_STATUS_OK);
    if (strcmp(operation, "revert") == 0)
        REQUIRE(umi_ai_coding_assistant_apply_patch(f.service, "patch.example", &adapter) == UMI_STATUS_OK);
    f.reads = f.writes = f.removes = 0;
    f.service->revision = final_revision ? UINT64_MAX - 1U : UINT64_MAX;
    before = malloc(sizeof *before); REQUIRE(before != NULL);
    memcpy(before, f.service, sizeof *before);
    UmiStatus status = mutate(&f, operation, &adapter);
    if (final_revision) {
        REQUIRE(status == UMI_STATUS_OK && f.service->revision == UINT64_MAX);
        REQUIRE(coding_owner_validate(f.service) == UMI_STATUS_OK);
    } else {
        REQUIRE(status == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(before, f.service, sizeof *before) == 0);
        REQUIRE(f.reads == 0 && f.writes == 0 && f.removes == 0);
    }
    result = 0;
cleanup:
    free(before); fixture_clear(&f); return result;
}

static int reentry_case(const char *operation, const char *phase, int reverting)
{
    OwnershipFixture f = {0};
    int result = 1;
    REQUIRE(fixture_init(&f) == UMI_STATUS_OK);
    UmiAiCodingFileAdapter adapter = fixture_adapter(&f);
    REQUIRE(umi_ai_coding_assistant_approve_patch(f.service, "patch.example", "reviewer") == UMI_STATUS_OK);
    if (reverting) REQUIRE(umi_ai_coding_assistant_apply_patch(f.service, "patch.example", &adapter) == UMI_STATUS_OK);
    f.reads = f.writes = f.removes = 0;
    f.hook = operation; f.phase = phase; f.original_adapter = &adapter;
    f.fail_first_write = strcmp(phase, "compensation") == 0;
    uint64_t revision = f.service->revision;
    UmiStatus status = reverting ?
        umi_ai_coding_assistant_revert_patch(f.service, "patch.example", &adapter) :
        umi_ai_coding_assistant_apply_patch(f.service, "patch.example", &adapter);
    REQUIRE(f.entered && !f.hook_failed && !f.service->patch_transaction_active);
    if (f.fail_first_write) {
/* The service now observes the owned patch revision after any attempted mutation, including a compensated failure. The previous success-only revision expectation is retained. The former implementation is retained for owner review. */
#if 0
        REQUIRE(status == UMI_STATUS_IO_ERROR && f.service->revision == revision);
#endif
        REQUIRE(status == UMI_STATUS_IO_ERROR && f.service->revision == revision + 1U);
        REQUIRE(strcmp(f.text, reverting ? "after" : "before") == 0);
    } else {
        REQUIRE(status == UMI_STATUS_OK && f.service->revision == revision + 1U);
        REQUIRE(strcmp(f.text, reverting ? "before" : "after") == 0);
    }
    REQUIRE(coding_owner_validate(f.service) == UMI_STATUS_OK);
    /* The guard is released on success and failure; a later ordinary mutation works. */
    UmiAiCodingPatchPolicy policy = umi_ai_coding_patch_policy_default();
    REQUIRE(umi_ai_coding_assistant_set_patch_policy(f.service, &policy) == UMI_STATUS_OK);
    result = 0;
cleanup:
    fixture_clear(&f); return result;
}

static int owner_case(const char *mode)
{
    OwnershipFixture f = {0};
    UmiAiCodingAssistantSnapshot output, before;
    int result = 1;
    REQUIRE(fixture_init(&f) == UMI_STATUS_OK);
    if (strcmp(mode, "zero-revision") == 0) f.service->revision = 0U;
    else if (strcmp(mode, "task-count") == 0) f.service->task_count = SIZE_MAX;
    else if (strcmp(mode, "patch-count") == 0) f.service->patch_count = SIZE_MAX;
    else if (strcmp(mode, "task-text") == 0) memset(f.service->tasks[0].request.request_id, 'x', UMI_AI_ID_CAPACITY);
    else if (strcmp(mode, "task-revision") == 0) f.service->tasks[0].revision = 0U;
    else if (strcmp(mode, "task-ahead") == 0) f.service->tasks[0].revision = f.service->revision + 1U;
    else if (strcmp(mode, "task-file-count") == 0) f.service->tasks[0].repository_context.file_count = SIZE_MAX;
    else if (strcmp(mode, "patch-text") == 0) memset(f.service->patches[0].patch_id, 'x', UMI_AI_ID_CAPACITY);
    else if (strcmp(mode, "patch-duplicate") == 0) {
        f.service->patches[1] = f.service->patches[0]; f.service->patch_count = 2U;
    } else if (strcmp(mode, "policy") == 0) f.service->patch_policy.maximum_files = SIZE_MAX;
    else if (strcmp(mode, "read-final") == 0) {
        f.service->revision = UINT64_MAX;
        REQUIRE(umi_ai_coding_assistant_snapshot(f.service, &output) == UMI_STATUS_OK);
        REQUIRE(output.revision == UINT64_MAX);
        result = 0; goto cleanup;
    } else if (strcmp(mode, "query-bound") == 0) {
        char query[UMI_AI_ID_CAPACITY]; memset(query, 'x', sizeof query);
        REQUIRE(umi_ai_coding_assistant_find_patch(f.service, query, f.read_patch) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_ai_coding_assistant_approve_patch(f.service, query, "reviewer") == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_ai_coding_assistant_reject_patch(f.service, query) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_ai_coding_assistant_apply_patch(f.service, query, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_ai_coding_assistant_revert_patch(f.service, query, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        result = 0; goto cleanup;
    } else if (strcmp(mode, "adapter-invalid") == 0) {
        UmiAiCodingFileAdapter adapter = fixture_adapter(&f);
        REQUIRE(umi_ai_coding_assistant_approve_patch(f.service, "patch.example", "reviewer") == UMI_STATUS_OK);
        uint64_t revision = f.service->revision;
        REQUIRE(UmiAiCodingFileAdapterValidate(&adapter) == UMI_STATUS_OK);
        REQUIRE(UmiAiCodingFileAdapterValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_ai_coding_assistant_apply_patch(f.service, "patch.example", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        for (int fault = 0; fault < 5; ++fault) {
            adapter = fixture_adapter(&f);
            if (fault == 0) adapter.structure_size = 0U;
            else if (fault == 1) adapter.abi_version = 0U;
            else if (fault == 2) adapter.read = NULL;
            else if (fault == 3) adapter.write = NULL;
            else adapter.remove = NULL;
            REQUIRE(UmiAiCodingFileAdapterValidate(&adapter) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(umi_ai_coding_assistant_apply_patch(f.service, "patch.example", &adapter) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(!f.service->patch_transaction_active && f.service->revision == revision);
        }
        REQUIRE(f.reads == 0 && f.writes == 0 && f.removes == 0);
        result = 0; goto cleanup;
    } else if (strcmp(mode, "destroy-null") == 0) {
        REQUIRE(UmiAiCodingAssistantDestroyChecked(NULL) == UMI_STATUS_OK);
        result = 0; goto cleanup;
    } else if (strcmp(mode, "destroy-checked") == 0) {
        REQUIRE(UmiAiCodingAssistantDestroyChecked(f.service) == UMI_STATUS_OK); f.service = NULL;
        result = 0; goto cleanup;
    } else { fprintf(stderr, "unknown owner case: %s\n", mode); goto cleanup; }
    memset(&output, 0x5a, sizeof output); memcpy(&before, &output, sizeof before);
    REQUIRE(umi_ai_coding_assistant_snapshot(f.service, &output) == UMI_STATUS_INVALID_STATE);
    REQUIRE(memcmp(&before, &output, sizeof before) == 0);
    REQUIRE(umi_ai_coding_assistant_task_at(f.service, 0U, f.plan) == UMI_STATUS_INVALID_STATE);
    REQUIRE(umi_ai_coding_assistant_find_patch(f.service, "patch.example", f.read_patch) == UMI_STATUS_INVALID_STATE);
    UmiAiCodingPatchPolicy policy = umi_ai_coding_patch_policy_default();
    REQUIRE(umi_ai_coding_assistant_set_patch_policy(f.service, &policy) == UMI_STATUS_INVALID_STATE);
    result = 0;
cleanup:
    fixture_clear(&f); return result;
}

static int report_case(const char *mode, int reverting)
{
    OwnershipFixture f = {0};
    UmiAiCodingPatchTransactionReport report, before;
    UmiAiCodingFileAdapter adapter;
    int result = 1;
    REQUIRE(fixture_init(&f) == UMI_STATUS_OK);
    adapter = fixture_adapter(&f);
    REQUIRE(umi_ai_coding_assistant_approve_patch(f.service, "patch.example", "reviewer") == UMI_STATUS_OK);
    if (reverting) REQUIRE(umi_ai_coding_assistant_apply_patch(f.service, "patch.example", &adapter) == UMI_STATUS_OK);
    f.reads = f.writes = f.removes = 0;
    const char *id = "patch.example";
    UmiStatus expected = UMI_STATUS_OK;
    int admitted = 1, mutation = 1;
    if (strcmp(mode, "failure") == 0 || strcmp(mode, "compensation") == 0 || strcmp(mode, "readback") == 0) {
        f.fail_first_write = 1; expected = UMI_STATUS_IO_ERROR;
        f.fail_compensation_write = strcmp(mode, "compensation") == 0;
        f.fail_restoration_read = strcmp(mode, "readback") == 0;
    } else if (strcmp(mode, "conflict") == 0) { strcpy(f.text, "external change"); expected = UMI_STATUS_INVALID_STATE; mutation = 0; }
    else if (strcmp(mode, "success") != 0) {
        admitted = mutation = 0;
        if (strcmp(mode, "null-report") == 0) expected = UMI_STATUS_INVALID_ARGUMENT;
        else if (strcmp(mode, "missing") == 0) { id = "missing"; expected = UMI_STATUS_NOT_FOUND; }
        else if (strcmp(mode, "adapter") == 0) { adapter.structure_size = 0U; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (strcmp(mode, "busy") == 0) { f.service->patch_transaction_active = 1; expected = UMI_STATUS_BUSY; }
        else if (strcmp(mode, "exhausted") == 0) { f.service->revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
        else if (strcmp(mode, "patch-exhausted") == 0) { f.service->patches[0].revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
        else { fprintf(stderr, "unknown report case: %s\n", mode); goto cleanup; }
    }
    uint64_t revision = f.service->revision;
    memset(&report, 0x5a, sizeof(report)); memcpy(&before, &report, sizeof(report));
    UmiStatus status = reverting ? UmiAiCodingAssistantRevertPatchWithReport(f.service, id, &adapter,
        strcmp(mode, "null-report") == 0 ? NULL : &report) :
        UmiAiCodingAssistantApplyPatchWithReport(f.service, id, &adapter,
        strcmp(mode, "null-report") == 0 ? NULL : &report);
    REQUIRE(status == expected);
    REQUIRE(f.service->revision == revision + (uint64_t)mutation);
    if (admitted) {
        REQUIRE(report.admitted == 1 && report.primary_status == expected);
        REQUIRE(report.mutation_attempts == (size_t)mutation);
        if (f.fail_first_write) {
            REQUIRE(report.compensation_attempts == 1U && report.restoration_checks == 1U);
            REQUIRE(report.compensation_failures == (size_t)f.fail_compensation_write);
            REQUIRE(report.restoration_failures == (size_t)f.fail_restoration_read);
            REQUIRE(report.compensation_status == (f.fail_compensation_write ? UMI_STATUS_PERMISSION_DENIED :
                f.fail_restoration_read ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK));
        }
        REQUIRE(!f.service->patch_transaction_active);
    } else {
        REQUIRE(memcmp(&report, &before, sizeof(report)) == 0);
        REQUIRE(f.reads == 0 && f.writes == 0 && f.removes == 0);
    }
    result = 0;
cleanup:
    if (f.service != NULL) f.service->patch_transaction_active = 0;
    fixture_clear(&f); return result;
}

int main(int argc, char **argv)
{
    if (argc == 4 && strcmp(argv[1], "report") == 0) return report_case(argv[2], strcmp(argv[3], "revert") == 0);
    if (argc == 3 && strcmp(argv[1], "owner") == 0) return owner_case(argv[2]);
    if (argc == 3 && strcmp(argv[1], "max") == 0) return mutation_case(argv[2], 0);
    if (argc == 3 && strcmp(argv[1], "last") == 0) return mutation_case(argv[2], 1);
    if (argc == 5 && strcmp(argv[1], "reentry") == 0)
        return reentry_case(argv[2], argv[3], strcmp(argv[4], "revert") == 0);
    return 2;
}
