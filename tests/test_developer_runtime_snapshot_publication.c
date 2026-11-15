/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_runtime_snapshot_publication.c
 *
 * PURPOSE:
 *   Verify complete owned and bound runtime observations, late child-read failures, allocation refusal and recovery using real Framework child services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "support/assertions.h"
#include "umicom/developer/developer.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static unsigned fault_child, observed_count, observed_order[9], allocation_calls;
static int recording, refuse_allocation;

/* Read real child services, then inject a refusal after their local output has
 * been written. The parent must not leak even this late partial observation. */
#define OBSERVE(name, owner_type, snapshot_type, real_read, number) \
    static UmiStatus name(owner_type *owner, snapshot_type *out) { \
        if (recording) { assert(observed_count < 9U); observed_order[observed_count++] = number; } \
        UmiStatus status = real_read(owner, out); \
        if (status == UMI_STATUS_OK && fault_child == number) { \
            memset(out, 0xa7, sizeof(*out)); return UMI_STATUS_IO_ERROR; \
        } \
        return status; \
    }
OBSERVE(ReadContext, const UmiDeveloperContext, UmiDeveloperContextSnapshot, umi_developer_context_snapshot, 1U)
OBSERVE(ReadPipeline, UmiDeveloperPipeline, UmiDeveloperPipelineSnapshot, umi_developer_pipeline_snapshot, 2U)
OBSERVE(ReadJournal, const UmiDeveloperJournal, UmiDeveloperJournalSnapshot, umi_developer_journal_snapshot, 3U)
OBSERVE(ReadProjects, const UmiProjectWorkspace, UmiProjectWorkspaceSnapshot, umi_project_workspace_snapshot, 4U)
OBSERVE(ReadLanguage, const UmiLanguageService, UmiLanguageServiceSnapshot, umi_language_service_snapshot, 5U)
OBSERVE(ReadDebug, const UmiDebugService, UmiDebugServiceSnapshot, umi_debug_service_snapshot, 6U)
OBSERVE(ReadSourceControl, const UmiSourceControlService, UmiSourceControlServiceSnapshot, umi_source_control_service_snapshot, 7U)
OBSERVE(ReadTests, const UmiTestPlatformService, UmiTestPlatformServiceSnapshot, umi_test_platform_service_snapshot, 8U)
OBSERVE(ReadWorkbench, const UmiUiWorkbenchServices, UmiUiWorkbenchServicesSnapshot, umi_ui_workbench_services_snapshot, 9U)
#undef OBSERVE

static void *RuntimeAllocate(size_t count, size_t size)
{
    if (recording) ++allocation_calls;
    return refuse_allocation ? NULL : calloc(count, size);
}
#define umi_developer_context_snapshot ReadContext
#define umi_developer_pipeline_snapshot ReadPipeline
#define umi_developer_journal_snapshot ReadJournal
#define umi_project_workspace_snapshot ReadProjects
#define umi_language_service_snapshot ReadLanguage
#define umi_debug_service_snapshot ReadDebug
#define umi_source_control_service_snapshot ReadSourceControl
#define umi_test_platform_service_snapshot ReadTests
#define umi_ui_workbench_services_snapshot ReadWorkbench
#define calloc RuntimeAllocate
#include "../src/developer/runtime.c"
#undef calloc
#undef umi_ui_workbench_services_snapshot
#undef umi_test_platform_service_snapshot
#undef umi_source_control_service_snapshot
#undef umi_debug_service_snapshot
#undef umi_language_service_snapshot
#undef umi_project_workspace_snapshot
#undef umi_developer_journal_snapshot
#undef umi_developer_pipeline_snapshot
#undef umi_developer_context_snapshot

static void Seed(UmiDeveloperRuntime *runtime)
{
    UmiDeveloperContextSnapshot context = {0};
    strcpy(context.project_id, "observation.fixture");
    strcpy(context.active_document, "draft.c");
    assert(umi_developer_runtime_set_context(runtime, &context) == UMI_STATUS_OK);
    UmiDeveloperOperationSnapshot operation;
    assert(umi_developer_operation_init(&operation, "queued.fixture",
        UMI_DEVELOPER_OPERATION_CUSTOM, "Queued fixture") == UMI_STATUS_OK);
    assert(umi_developer_operation_set_program(&operation, "synthetic-tool", NULL) == UMI_STATUS_OK);
    assert(umi_developer_runtime_submit_operation(runtime, &operation) == UMI_STATUS_OK);
}
static UmiDeveloperRuntime *Bind(UmiDeveloperRuntime *owner)
{
    UmiDeveloperRuntimeBindings bindings;
    umi_developer_runtime_bindings_init(&bindings);
    bindings.projects = umi_developer_runtime_projects(owner);
    bindings.language = umi_developer_runtime_language(owner);
    bindings.debug = umi_developer_runtime_debug(owner);
    bindings.source_control = umi_developer_runtime_source_control(owner);
    bindings.tests = umi_developer_runtime_tests(owner);
    bindings.workbench = umi_developer_runtime_workbench(owner);
    UmiDeveloperRuntime *bound = NULL;
    assert(umi_developer_runtime_create_bound(&bindings, &bound) == UMI_STATUS_OK);
    return bound;
}
static void CheckOrder(unsigned expected)
{
    assert(observed_count == expected);
    for (unsigned i = 0U; i < expected; ++i) assert(observed_order[i] == i + 1U);
}
int main(int argc, char **argv)
{
    if (argc != 3 || (strcmp(argv[1], "owned") && strcmp(argv[1], "bound"))) return 2;
    static const char *children[] = {
        "context", "pipeline", "journal", "projects", "language",
        "debug", "source-control", "tests", "workbench"
    };
    UmiDeveloperRuntime *owner = NULL;
    assert(umi_developer_runtime_create_owned(&owner) == UMI_STATUS_OK);
    int bound = !strcmp(argv[1], "bound");
    UmiDeveloperRuntime *runtime = bound ? Bind(owner) : owner;
    Seed(runtime);
    UmiDeveloperRuntimeSnapshot *baseline = malloc(sizeof(*baseline));
    UmiDeveloperRuntimeSnapshot *output = malloc(sizeof(*output)), *sentinel = malloc(sizeof(*sentinel));
    assert(baseline != NULL && output != NULL && sentinel != NULL);
    assert(umi_developer_runtime_snapshot(runtime, baseline) == UMI_STATUS_OK);
    assert(baseline->owns_services == !bound);
    assert(!strcmp(baseline->context.project_id, "observation.fixture"));
    assert(baseline->pipeline.operation_count == 1U && baseline->journal.entry_count == 2U);
    for (unsigned i = 0U; i < sizeof(children) / sizeof(children[0]); ++i)
        if (!strcmp(argv[2], children[i])) fault_child = i + 1U;
    refuse_allocation = !strcmp(argv[2], "allocation");
    int null_owner = !strcmp(argv[2], "null-runtime"), null_output = !strcmp(argv[2], "null-output");
    int success = !strcmp(argv[2], "success");
    assert(success || fault_child != 0U || refuse_allocation || null_owner || null_output);
    memset(output, 0x5a, sizeof(*output)); memcpy(sentinel, output, sizeof(*output));
    recording = 1;
    UmiStatus status = umi_developer_runtime_snapshot(
        null_owner ? NULL : runtime, null_output ? NULL : output);
    UmiStatus expected = success ? UMI_STATUS_OK : fault_child != 0U ? UMI_STATUS_IO_ERROR :
        refuse_allocation ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_INVALID_ARGUMENT;
    assert(status == expected);
    if (success) assert(memcmp(output, baseline, sizeof(*output)) == 0);
    else assert(memcmp(output, sentinel, sizeof(*output)) == 0);
    CheckOrder(success ? 9U : fault_child);
    assert(allocation_calls == (null_owner || null_output ? 0U : 1U));

    /* A refused read changes neither the runtime nor its children. Removing
     * the injected fault must recover the complete original observation. */
    fault_child = 0U; refuse_allocation = 0; observed_count = 0U; allocation_calls = 0U;
    assert(umi_developer_runtime_snapshot(runtime, output) == UMI_STATUS_OK);
    CheckOrder(9U);
    assert(memcmp(output, baseline, sizeof(*output)) == 0);
    recording = 0;
    if (bound) {
        umi_developer_runtime_destroy(runtime);
        assert(umi_developer_runtime_snapshot(owner, output) == UMI_STATUS_OK);
        assert(output->owns_services == 1);
    }
    umi_developer_runtime_destroy(owner);
    free(baseline); free(output); free(sentinel);
    return 0;
}
