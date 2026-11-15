/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_project_workflow_transaction.c
 *
 * PURPOSE:
 *   Verify project context and queued workflows publish together, including late refusal, retry, dependency order and identity exhaustion.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *budget_fault = "none";
static UmiStatus ObservedJournal(const UmiDeveloperJournal *journal,
    UmiDeveloperJournalSnapshot *out);

/* Compile the actual owners here to reach counter boundaries without billions
 * of mutations. Only journal observation is injected; queue publication and
 * event storage remain the production implementations. */
#include "../src/developer/context.c"
#define umi_developer_journal_snapshot ObservedJournal
#include "../src/developer/runtime.c"
#undef umi_developer_journal_snapshot

static UmiStatus ObservedJournal(const UmiDeveloperJournal *journal,
    UmiDeveloperJournalSnapshot *out)
{
    UmiStatus status = umi_developer_journal_snapshot(journal, out);
    if (status != UMI_STATUS_OK) return status;
    if (!strcmp(budget_fault, "sequence")) out->next_sequence = UINT64_MAX;
    if (!strcmp(budget_fault, "journal")) out->revision = UINT64_MAX;
    if (!strcmp(budget_fault, "dropped")) {
        out->entry_count = UMI_DEVELOPER_JOURNAL_CAPACITY;
        out->dropped_count = UINT64_MAX;
    }
    return status;
}

typedef struct Captured {
    UmiDeveloperRuntimeSnapshot runtime;
    UmiDeveloperOperationSnapshot *operations;
    UmiDeveloperJournalEntry *events;
} Captured;

static Captured *Capture(UmiDeveloperRuntime *runtime)
{
    Captured *value = calloc(1U, sizeof(*value));
    assert(value != NULL);
    /* Capture actual state; injected admission budgets are not stored entries. */
    const char *saved_fault = budget_fault;
    budget_fault = "none";
    assert(umi_developer_runtime_snapshot(runtime, &value->runtime) == UMI_STATUS_OK);
    budget_fault = saved_fault;
    size_t count = value->runtime.pipeline.operation_count;
    value->operations = calloc(count ? count : 1U, sizeof(*value->operations));
    count = value->runtime.journal.entry_count;
    value->events = calloc(count ? count : 1U, sizeof(*value->events));
    assert(value->operations != NULL && value->events != NULL);
    for (size_t i = 0U; i < value->runtime.pipeline.operation_count; ++i)
        assert(umi_developer_pipeline_at(runtime->pipeline, i, &value->operations[i]) == UMI_STATUS_OK);
    for (size_t i = 0U; i < value->runtime.journal.entry_count; ++i)
        assert(umi_developer_journal_at(runtime->journal, i, &value->events[i]) == UMI_STATUS_OK);
    return value;
}

static void FreeCapture(Captured *value)
{ free(value->operations); free(value->events); free(value); }

static void Unchanged(UmiDeveloperRuntime *runtime, const Captured *before)
{
    Captured *after = Capture(runtime);
    assert(memcmp(&before->runtime, &after->runtime, sizeof(before->runtime)) == 0);
    assert(memcmp(before->operations, after->operations,
        before->runtime.pipeline.operation_count * sizeof(*before->operations)) == 0);
    assert(memcmp(before->events, after->events,
        before->runtime.journal.entry_count * sizeof(*before->events)) == 0);
    FreeCapture(after);
}

static UmiDeveloperOperationSnapshot Operation(const char *id)
{
    UmiDeveloperOperationSnapshot operation;
    assert(umi_developer_operation_init(&operation, id,
        UMI_DEVELOPER_OPERATION_CUSTOM, id) == UMI_STATUS_OK);
    assert(umi_developer_operation_set_program(&operation, "synthetic-tool", NULL) == UMI_STATUS_OK);
    return operation;
}

static UmiDeveloperRuntime *Runtime(int existing)
{
    budget_fault = "none";
    UmiDeveloperRuntime *runtime = NULL;
    assert(umi_developer_runtime_create_owned(&runtime) == UMI_STATUS_OK);
    UmiDeveloperContextSnapshot context = {0};
    strcpy(context.project_id, "previous");
    strcpy(context.active_document, "unsaved-notes.c");
    assert(umi_developer_runtime_set_context(runtime, &context) == UMI_STATUS_OK);
    if (existing) {
        UmiDeveloperOperationSnapshot operation = Operation("existing");
        assert(umi_developer_runtime_submit_operation(runtime, &operation) == UMI_STATUS_OK);
        assert(umi_developer_runtime_start_operation(runtime, "existing") == UMI_STATUS_OK);
    }
    return runtime;
}

static void Task(UmiDeveloperRuntime *runtime, const char *id, const char *command)
{
    UmiProjectTaskSnapshot task = {0};
    task.struct_size = (uint32_t)sizeof(task);
    task.api_version = UMI_PROJECT_TASK_API_VERSION;
    strcpy(task.project_id, "fixture"); strcpy(task.id, id);
    strcpy(task.label, id); strcpy(task.group, id); strcpy(task.command, command);
    task.enabled = 1;
    task.default_task = !strcmp(id, "build");
    assert(umi_project_task_registry_upsert(
        umi_project_workspace_task(runtime->projects), &task) == UMI_STATUS_OK);
}

static void Project(UmiDeveloperRuntime *runtime, const char *launch_arguments)
{
    UmiProjectDescriptorSnapshot project = {0};
    project.struct_size = (uint32_t)sizeof(project);
    project.api_version = UMI_PROJECT_DESCRIPTOR_API_VERSION;
    strcpy(project.id, "fixture"); strcpy(project.name, "Workflow fixture");
    strcpy(project.root_uri, "fixtures/project with spaces"); project.enabled = 1;
    assert(umi_project_descriptor_registry_upsert(
        umi_project_workspace_descriptor(runtime->projects), &project) == UMI_STATUS_OK);
    Task(runtime, "configure", "synthetic-tool configure");
    Task(runtime, "build", "synthetic-tool build");
    Task(runtime, "test", "synthetic-tool test");
    if (launch_arguments != NULL) {
        UmiProjectLaunchProfileSnapshot launch = {0};
        launch.struct_size = (uint32_t)sizeof(launch);
        launch.api_version = UMI_PROJECT_LAUNCH_PROFILE_API_VERSION;
        strcpy(launch.id, "launch"); strcpy(launch.project_id, "fixture");
        strcpy(launch.name, "Run fixture"); strcpy(launch.program, "synthetic-tool");
        strcpy(launch.arguments, launch_arguments); launch.default_profile = 1;
        assert(umi_project_launch_profile_registry_upsert(
            umi_project_workspace_launch_profile(runtime->projects), &launch) == UMI_STATUS_OK);
    }
}

static void Published(UmiDeveloperRuntime *runtime, const Captured *before,
    const char *project_id, const char *const *ids, size_t count)
{
    Captured *after = Capture(runtime);
    assert(!strcmp(after->runtime.context.project_id, project_id));
    assert(after->runtime.context.revision == before->runtime.context.revision + 1U);
    assert(after->runtime.revision == before->runtime.revision + 2U * count);
    assert(after->runtime.pipeline.operation_count == before->runtime.pipeline.operation_count + count);
    assert(after->runtime.pipeline.dependency_count == before->runtime.pipeline.dependency_count + count - 1U);
    assert(after->runtime.journal.entry_count == before->runtime.journal.entry_count + 2U * count);
    assert(memcmp(before->operations, after->operations,
        before->runtime.pipeline.operation_count * sizeof(*before->operations)) == 0);
    size_t event = before->runtime.journal.entry_count;
    assert(after->events[event].kind == UMI_DEVELOPER_EVENT_CONTEXT_CHANGED);
    assert(after->events[event++].operation_id[0] == '\0');
    for (size_t i = 0U; i < count; ++i) {
        assert(after->events[event].kind == UMI_DEVELOPER_EVENT_SUBMITTED);
        assert(!strcmp(after->events[event++].operation_id, ids[i]));
        if (i != 0U) {
            assert(after->events[event].kind == UMI_DEVELOPER_EVENT_DEPENDENCY_ADDED);
            assert(!strcmp(after->events[event++].operation_id, ids[i]));
        }
    }
    FreeCapture(after);
}

static void ProjectWorkflow(const char *mode)
{
    UmiDeveloperRuntime *runtime = Runtime(1);
    Project(runtime, !strcmp(mode, "missing-launch") ? NULL :
        !strcmp(mode, "malformed-launch") ? "\"unterminated" : "--sample");
    UmiDeveloperProjectWorkflowRequest request = {0};
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_PROJECT_WORKFLOW_API_VERSION;
    request.project_id = "fixture"; request.workflow_id = "project.batch";
    request.include_configure = 1;
    request.preset = UMI_DEVELOPER_PROJECT_WORKFLOW_FULL;
    int success = 1;
    size_t count = 4U;
    if (!strcmp(mode, "build")) { request.preset = UMI_DEVELOPER_PROJECT_WORKFLOW_BUILD; count = 2U; }
    else if (!strcmp(mode, "test")) { request.preset = UMI_DEVELOPER_PROJECT_WORKFLOW_TEST; count = 3U; }
    else if (!strcmp(mode, "run")) { request.preset = UMI_DEVELOPER_PROJECT_WORKFLOW_RUN; count = 3U; }
    else if (!strcmp(mode, "debug")) { request.preset = UMI_DEVELOPER_PROJECT_WORKFLOW_DEBUG; count = 3U; }
    else if (!strcmp(mode, "without-configure")) { request.include_configure = 0; count = 3U; }
    else if (!strcmp(mode, "malformed-task") || !strcmp(mode, "repair")) {
        Task(runtime, "test", "synthetic-tool \"unterminated"); success = 0;
    } else if (!strcmp(mode, "collision")) {
        UmiDeveloperOperationSnapshot operation = Operation("project.batch.task.2");
        assert(umi_developer_runtime_submit_operation(runtime, &operation) == UMI_STATUS_OK);
        success = 0;
    } else if (!strcmp(mode, "capacity")) {
        for (size_t i = 1U; i < UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY - 2U; ++i) {
            char id[64]; snprintf(id, sizeof(id), "occupied.%zu", i);
            UmiDeveloperOperationSnapshot operation = Operation(id);
            assert(umi_developer_pipeline_submit(runtime->pipeline, &operation) == UMI_STATUS_OK);
        }
        success = 0;
    } else if (!strcmp(mode, "missing-launch") || !strcmp(mode, "malformed-launch")) success = 0;
    else assert(!strcmp(mode, "full"));
    Captured *before = Capture(runtime);
    UmiDeveloperProjectWorkflowSnapshot *output = malloc(sizeof(*output)), *sentinel = malloc(sizeof(*sentinel));
    assert(output != NULL && sentinel != NULL);
    memset(output, 0x5a, sizeof(*output)); memcpy(sentinel, output, sizeof(*output));
    UmiStatus status = umi_developer_project_workflow_submit(runtime, &request, output);
    if (!success) {
        assert(status != UMI_STATUS_OK);
        assert(memcmp(output, sentinel, sizeof(*output)) == 0);
        Unchanged(runtime, before);
        if (!strcmp(mode, "repair")) {
            Task(runtime, "test", "synthetic-tool corrected");
            /* Registry repair changes workspace identity, so capture the new
             * baseline before retrying the exact same workflow identity. */
            FreeCapture(before); before = Capture(runtime);
            status = umi_developer_project_workflow_submit(runtime, &request, output);
            assert(status == UMI_STATUS_OK); success = 1;
        }
    }
    if (success) {
        assert(status == UMI_STATUS_OK && output->workflow.operation_count == count);
        const char *ids[4];
        for (size_t i = 0U; i < count; ++i) ids[i] = output->workflow.operation_ids[i];
        Published(runtime, before, "fixture", ids, count);
        UmiDeveloperContextSnapshot context;
        assert(umi_developer_context_snapshot(runtime->context, &context) == UMI_STATUS_OK);
        assert(!strcmp(context.workspace_directory, "fixtures/project with spaces"));
        assert(context.active_document[0] == '\0');
        /* Model completions prove dependency order without spawning tools. */
        for (size_t i = 0U; i < count; ++i) {
            UmiDeveloperOperationSnapshot ready;
            assert(umi_developer_runtime_start_next(runtime, &ready) == UMI_STATUS_OK);
            assert(!strcmp(ready.id, ids[i]));
            if (!strcmp(mode, "debug") && i + 1U == count)
                assert(ready.kind == UMI_DEVELOPER_OPERATION_DEBUG);
            assert(umi_developer_runtime_complete_operation(runtime, ready.id, 0, "Synthetic") == UMI_STATUS_OK);
        }
    }
    free(output); free(sentinel); FreeCapture(before); umi_developer_runtime_destroy(runtime);
}

static void ContextChain(const char *mode)
{
    int maximum = !strcmp(mode, "maximum");
    UmiDeveloperRuntime *runtime = Runtime(!maximum);
    size_t count = maximum ? UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY : 2U;
    UmiDeveloperOperationSnapshot *operations = calloc(count, sizeof(*operations));
    const char **ids = calloc(count, sizeof(*ids));
    assert(operations != NULL && ids != NULL);
    for (size_t i = 0U; i < count; ++i) {
        char id[64]; snprintf(id, sizeof(id), "direct.%zu", i);
        operations[i] = Operation(id); ids[i] = operations[i].id;
    }
    UmiDeveloperContextSnapshot context = {0};
    strcpy(context.project_id, "selected");
    strcpy(context.active_document, "selected.c");
    int success = !strcmp(mode, "copies") || maximum || !strcmp(mode, "runtime-boundary") || !strcmp(mode, "context-boundary");
    if (!strcmp(mode, "duplicate")) strcpy(operations[1].id, operations[0].id);
    else if (!strcmp(mode, "malformed")) memset(operations[1].program, 'x', sizeof(operations[1].program));
    else if (!strcmp(mode, "runtime")) runtime->revision = UINT64_MAX - 3U;
    else if (!strcmp(mode, "runtime-boundary")) runtime->revision = UINT64_MAX - 4U;
    else if (!strcmp(mode, "context")) runtime->context->state.revision = UINT64_MAX;
    else if (!strcmp(mode, "context-boundary")) runtime->context->state.revision = UINT64_MAX - 1U;
    else if (!strcmp(mode, "sequence") || !strcmp(mode, "journal") || !strcmp(mode, "dropped")) budget_fault = mode;
    else if (!success && strcmp(mode, "null-context") && strcmp(mode, "null-operations") &&
        strcmp(mode, "null-runtime") && strcmp(mode, "empty") && strcmp(mode, "oversized"))
        assert(!"unknown direct transaction scenario");
    Captured *before = Capture(runtime);
    UmiStatus status = umi_developer_runtime_submit_context_chain(
        !strcmp(mode, "null-runtime") ? NULL : runtime,
        !strcmp(mode, "null-context") ? NULL : &context,
        !strcmp(mode, "null-operations") ? NULL : operations,
        !strcmp(mode, "empty") ? 0U : !strcmp(mode, "oversized") ?
            UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY + 1U : count);
    if (success) {
        assert(status == UMI_STATUS_OK);
        Published(runtime, before, "selected", ids, count);
        memset(&context, 0, sizeof(context));
        memset(operations, 0, count * sizeof(*operations));
        UmiDeveloperContextSnapshot observed;
        assert(umi_developer_context_snapshot(runtime->context, &observed) == UMI_STATUS_OK);
        assert(!strcmp(observed.project_id, "selected") && !strcmp(observed.active_document, "selected.c"));
        UmiDeveloperOperationSnapshot operation;
        assert(umi_developer_pipeline_find(runtime->pipeline, "direct.1", &operation) == UMI_STATUS_OK);
        assert(!strcmp(operation.program, "synthetic-tool"));
    } else { assert(status != UMI_STATUS_OK); Unchanged(runtime, before); }
    budget_fault = "none";
    FreeCapture(before); free(ids); free(operations); umi_developer_runtime_destroy(runtime);
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!strncmp(argv[1], "project-", 8U)) ProjectWorkflow(argv[1] + 8U);
    else if (!strncmp(argv[1], "chain-", 6U)) ContextChain(argv[1] + 6U);
    else return 2;
    return 0;
}
