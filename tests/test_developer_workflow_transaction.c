/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_workflow_transaction.c
 *
 * PURPOSE:
 *   Verify complete workflow admission preserves queued work and evidence on every refused replacement.
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

typedef struct Captured {
    UmiDeveloperRuntimeSnapshot runtime;
    UmiDeveloperOperationSnapshot *operations;
    UmiDeveloperJournalEntry *events;
} Captured;
static UmiDeveloperOperationSnapshot Operation(const char *id)
{
    UmiDeveloperOperationSnapshot value;
    assert(umi_developer_operation_init(&value, id, UMI_DEVELOPER_OPERATION_CUSTOM, id) == UMI_STATUS_OK);
    assert(umi_developer_operation_set_program(&value, "synthetic-tool", NULL) == UMI_STATUS_OK);
    return value;
}
static void Task(UmiDeveloperRuntime *runtime, const char *id, const char *command, int enabled)
{
    UmiProjectTaskSnapshot task = {0};
    task.struct_size = (uint32_t)sizeof(task);
    task.api_version = UMI_PROJECT_TASK_API_VERSION;
    strcpy(task.project_id, "fixture");
    strcpy(task.group, "build");
    strcpy(task.id, id); strcpy(task.label, id); strcpy(task.command, command);
    task.enabled = enabled;
    assert(umi_project_task_registry_upsert(
        umi_project_workspace_task(umi_developer_runtime_projects(runtime)), &task) == UMI_STATUS_OK);
}
static void Launch(UmiDeveloperRuntime *runtime, const char *arguments)
{
    UmiProjectLaunchProfileSnapshot profile = {0};
    profile.struct_size = (uint32_t)sizeof(profile);
    profile.api_version = UMI_PROJECT_LAUNCH_PROFILE_API_VERSION;
    strcpy(profile.id, "launch"); strcpy(profile.project_id, "fixture");
    strcpy(profile.name, "Launch fixture"); strcpy(profile.program, "synthetic-tool");
    strcpy(profile.arguments, arguments);
    assert(umi_project_launch_profile_registry_upsert(
        umi_project_workspace_launch_profile(umi_developer_runtime_projects(runtime)), &profile) == UMI_STATUS_OK);
}
static UmiDeveloperRuntime *Runtime(void)
{
    UmiDeveloperRuntime *runtime = NULL;
    assert(umi_developer_runtime_create_owned(&runtime) == UMI_STATUS_OK);
    Task(runtime, "first", "synthetic-tool first", 1);
    Task(runtime, "second", "synthetic-tool second", 1);
    Launch(runtime, "--example");
    UmiDeveloperOperationSnapshot existing = Operation("existing");
    assert(umi_developer_runtime_submit_operation(runtime, &existing) == UMI_STATUS_OK);
    assert(umi_developer_runtime_start_operation(runtime, "existing") == UMI_STATUS_OK);
    return runtime;
}
static Captured *Capture(UmiDeveloperRuntime *runtime)
{
    Captured *value = calloc(1U, sizeof(*value));
    assert(value != NULL);
    assert(umi_developer_runtime_snapshot(runtime, &value->runtime) == UMI_STATUS_OK);
    size_t count = value->runtime.pipeline.operation_count;
    value->operations = calloc(count != 0U ? count : 1U, sizeof(*value->operations));
    count = value->runtime.journal.entry_count;
    value->events = calloc(count != 0U ? count : 1U, sizeof(*value->events));
    assert(value->operations != NULL && value->events != NULL);
    for (size_t i = 0U; i < value->runtime.pipeline.operation_count; ++i)
        assert(umi_developer_pipeline_at(umi_developer_runtime_pipeline(runtime), i, &value->operations[i]) == UMI_STATUS_OK);
    for (size_t i = 0U; i < value->runtime.journal.entry_count; ++i)
        assert(umi_developer_journal_at(umi_developer_runtime_journal(runtime), i, &value->events[i]) == UMI_STATUS_OK);
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
static void FillOperations(UmiDeveloperRuntime *runtime, size_t target)
{
    UmiDeveloperPipeline *pipeline = umi_developer_runtime_pipeline(runtime);
    while (umi_developer_pipeline_count(pipeline) < target) {
        char id[64];
        snprintf(id, sizeof(id), "occupied.%zu", umi_developer_pipeline_count(pipeline));
        UmiDeveloperOperationSnapshot value = Operation(id);
        assert(umi_developer_pipeline_submit(pipeline, &value) == UMI_STATUS_OK);
    }
}
static void FillDependencies(UmiDeveloperRuntime *runtime)
{
    FillOperations(runtime, 48U);
    UmiDeveloperPipeline *pipeline = umi_developer_runtime_pipeline(runtime);
    size_t edges = 0U;
    /* Index zero is running and remains independent. Edges point only backward
     * among queued synthetic operations, producing a finite acyclic fixture. */
    for (size_t i = 2U; i < 48U && edges < UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY; ++i)
        for (size_t j = 1U; j < i && edges < UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY; ++j) {
            char a[64], b[64];
            snprintf(a, sizeof(a), "occupied.%zu", i);
            snprintf(b, sizeof(b), "occupied.%zu", j);
            assert(umi_developer_pipeline_add_dependency(pipeline, a, b) == UMI_STATUS_OK);
            ++edges;
        }
    assert(edges == UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY);
}
static void Workflow(const char *mode)
{
    UmiDeveloperRuntime *runtime = Runtime();
    const char *tasks[UMI_DEVELOPER_WORKFLOW_MAX_TASKS];
    for (size_t i = 0U; i < UMI_DEVELOPER_WORKFLOW_MAX_TASKS; ++i) tasks[i] = "first";
    tasks[1] = "second";
    UmiDeveloperWorkflowRequest request = {0};
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_WORKFLOW_API_VERSION;
    request.workflow_id = "batch"; request.task_ids = tasks; request.task_count = 2U;
    char long_id[UMI_DEVELOPER_ID_CAPACITY];
    memset(long_id, 'x', sizeof(long_id));
    long_id[UMI_DEVELOPER_ID_CAPACITY - 9U] = 0;
    int success = 0;
    if (!strcmp(mode, "success")) { request.launch_profile_id = "launch"; success = 1; }
    else if (!strcmp(mode, "tasks-only")) success = 1;
    else if (!strcmp(mode, "launch-only")) { request.task_count = 0U; request.launch_profile_id = "launch"; success = 1; }
    else if (!strcmp(mode, "maximum")) { request.task_count = UMI_DEVELOPER_WORKFLOW_MAX_TASKS; request.launch_profile_id = "launch"; success = 1; }
    else if (!strcmp(mode, "repeat-task")) { tasks[1] = "first"; success = 1; }
    else if (!strcmp(mode, "missing-task") || !strcmp(mode, "repair-after-refusal")) tasks[1] = "missing";
    else if (!strcmp(mode, "late-id-limit")) { request.workflow_id = long_id; request.launch_profile_id = "launch"; }
    else if (!strcmp(mode, "disabled-task")) Task(runtime, "second", "synthetic-tool second", 0);
    else if (!strcmp(mode, "malformed-task")) Task(runtime, "second", "synthetic-tool \"unterminated", 1);
    else if (!strcmp(mode, "null-task")) tasks[1] = NULL;
    else if (!strcmp(mode, "empty-task")) tasks[1] = "";
    else if (!strcmp(mode, "missing-launch")) request.launch_profile_id = "missing";
    else if (!strcmp(mode, "malformed-launch")) { Launch(runtime, "\"unterminated"); request.launch_profile_id = "launch"; }
    else if (!strcmp(mode, "late-collision")) {
        UmiDeveloperOperationSnapshot collision = Operation("batch.task.2");
        assert(umi_developer_runtime_submit_operation(runtime, &collision) == UMI_STATUS_OK);
    } else if (!strcmp(mode, "operation-capacity")) FillOperations(runtime, UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY - 1U);
    else if (!strcmp(mode, "dependency-capacity")) FillDependencies(runtime);
    else if (!strcmp(mode, "empty-workflow")) request.task_count = 0U;
    else if (!strcmp(mode, "debug-range")) request.debug_mode = 2;
    else if (!strcmp(mode, "null-task-array")) request.task_ids = NULL;
    else if (!strcmp(mode, "task-count")) request.task_count = UMI_DEVELOPER_WORKFLOW_MAX_TASKS + 1U;
    else assert(!"unknown workflow scenario");
    Captured *before = Capture(runtime);
    UmiDeveloperWorkflowSnapshot output, sentinel;
    memset(&output, 0x5a, sizeof(output)); memcpy(&sentinel, &output, sizeof(sentinel));
    UmiStatus status = umi_developer_workflow_submit(runtime, &request, &output);
    if (!success) {
        assert(status != UMI_STATUS_OK);
        assert(memcmp(&output, &sentinel, sizeof(output)) == 0);
        Unchanged(runtime, before);
        if (!strcmp(mode, "repair-after-refusal")) {
            Task(runtime, "missing", "synthetic-tool corrected", 1);
            assert(umi_developer_workflow_submit(runtime, &request, &output) == UMI_STATUS_OK);
            assert(output.operation_count == 2U && output.dependency_count == 1U);
            assert(umi_developer_pipeline_count(umi_developer_runtime_pipeline(runtime)) == 3U);
        }
    } else {
        assert(status == UMI_STATUS_OK);
        size_t count = request.task_count + (request.launch_profile_id != NULL ? 1U : 0U);
        assert(output.operation_count == count && output.dependency_count == count - 1U);
        Captured *after = Capture(runtime);
        assert(after->runtime.pipeline.operation_count == before->runtime.pipeline.operation_count + count);
        assert(after->runtime.pipeline.dependency_count == before->runtime.pipeline.dependency_count + count - 1U);
        assert(after->runtime.journal.entry_count == before->runtime.journal.entry_count + count * 2U - 1U);
        assert(memcmp(&before->operations[0], &after->operations[0], sizeof(before->operations[0])) == 0);
        size_t event = before->runtime.journal.entry_count;
        for (size_t i = 0U; i < count; ++i) {
            assert(after->events[event].kind == UMI_DEVELOPER_EVENT_SUBMITTED);
            assert(!strcmp(after->events[event++].operation_id, output.operation_ids[i]));
            if (i != 0U) {
                assert(after->events[event].kind == UMI_DEVELOPER_EVENT_DEPENDENCY_ADDED);
                assert(!strcmp(after->events[event++].operation_id, output.operation_ids[i]));
            }
        }
        /* Observe the chain's ordering without executing any process. The
         * unrelated running operation remains running throughout this model. */
        for (size_t i = 0U; i < count; ++i) {
            UmiDeveloperOperationSnapshot ready;
            assert(umi_developer_runtime_start_next(runtime, &ready) == UMI_STATUS_OK);
            assert(!strcmp(ready.id, output.operation_ids[i]));
            assert(umi_developer_runtime_complete_operation(runtime, ready.id, 0, "Synthetic completion") == UMI_STATUS_OK);
        }
        FreeCapture(after);
    }
    FreeCapture(before);
    umi_developer_runtime_destroy(runtime);
}
static void DirectChain(const char *mode)
{
    UmiDeveloperRuntime *runtime = Runtime();
    UmiDeveloperOperationSnapshot *values = calloc(2U, sizeof(*values));
    assert(values != NULL);
    values[0] = Operation("direct.first"); values[1] = Operation("direct.second");
    size_t count = 2U;
    int success = !strcmp(mode, "chain-copies");
    if (!strcmp(mode, "chain-duplicate")) strcpy(values[1].id, values[0].id);
    else if (!strcmp(mode, "chain-existing")) strcpy(values[1].id, "existing");
    else if (!strcmp(mode, "chain-id")) memset(values[1].id, 'x', sizeof(values[1].id));
    else if (!strcmp(mode, "chain-title")) memset(values[1].title, 'x', sizeof(values[1].title));
    else if (!strcmp(mode, "chain-program")) memset(values[1].program, 'x', sizeof(values[1].program));
    else if (!strcmp(mode, "chain-summary")) memset(values[1].summary, 'x', sizeof(values[1].summary));
    else if (!strcmp(mode, "chain-argument")) { values[1].argument_count = 1U; memset(values[1].arguments[0], 'x', sizeof(values[1].arguments[0])); }
    else if (!strcmp(mode, "chain-argument-count")) values[1].argument_count = UMI_DEVELOPER_MAX_ARGUMENTS + 1U;
    else if (!strcmp(mode, "chain-struct-size")) values[1].struct_size = 0U;
    else if (!strcmp(mode, "chain-version")) values[1].api_version++;
    else if (!strcmp(mode, "chain-kind")) values[1].kind = (UmiDeveloperOperationKind)(UMI_DEVELOPER_OPERATION_AI + 1);
    else if (!strcmp(mode, "chain-zero")) count = 0U;
    else if (!strcmp(mode, "chain-count")) count = UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY + 1U;
    else if (strcmp(mode, "chain-null-array") && strcmp(mode, "chain-null-runtime") && !success)
        assert(!"unknown chain scenario");
    Captured *before = Capture(runtime);
    UmiStatus status = umi_developer_runtime_submit_chain(
        !strcmp(mode, "chain-null-runtime") ? NULL : runtime,
        !strcmp(mode, "chain-null-array") ? NULL : values, count);
    if (success) {
        assert(status == UMI_STATUS_OK);
        memset(values, 0, 2U * sizeof(*values));
        UmiDeveloperOperationSnapshot observed;
        assert(umi_developer_pipeline_find(umi_developer_runtime_pipeline(runtime), "direct.second", &observed) == UMI_STATUS_OK);
        assert(!strcmp(observed.program, "synthetic-tool"));
    } else { assert(status != UMI_STATUS_OK); Unchanged(runtime, before); }
    FreeCapture(before); free(values); umi_developer_runtime_destroy(runtime);
}
static void Planner(const char *mode)
{
    UmiDeveloperRuntime *runtime = Runtime();
    Captured *before = Capture(runtime);
    UmiDeveloperOperationSnapshot operation, sentinel;
    memset(&operation, 0x6b, sizeof(operation)); memcpy(&sentinel, &operation, sizeof(sentinel));
    int missing = strstr(mode, "missing") != NULL;
    UmiStatus status;
    if (!strncmp(mode, "task-", 5U)) {
        UmiDeveloperTaskPlanRequest request = {0};
        request.task_id = missing ? "missing" : "first";
        UmiDeveloperTaskPlanSnapshot plan, previous;
        memset(&plan, 0x7c, sizeof(plan)); memcpy(&previous, &plan, sizeof(plan));
        status = umi_developer_task_plan_build(runtime, &request, &operation, &plan);
        if (missing) assert(memcmp(&plan, &previous, sizeof(plan)) == 0);
        else assert(!strcmp(plan.task_id, "first") && !strcmp(operation.program, "synthetic-tool"));
    } else {
        UmiDeveloperLaunchPlanRequest request = {0};
        request.launch_profile_id = missing ? "missing" : "launch";
        request.debug_mode = 1;
        UmiDeveloperLaunchPlanSnapshot plan, previous;
        memset(&plan, 0x7c, sizeof(plan)); memcpy(&previous, &plan, sizeof(plan));
        status = umi_developer_launch_plan_build(runtime, &request, &operation, &plan);
        if (missing) assert(memcmp(&plan, &previous, sizeof(plan)) == 0);
        else assert(plan.operation_kind == UMI_DEVELOPER_OPERATION_DEBUG);
    }
    assert((status == UMI_STATUS_OK) == !missing);
    if (missing) assert(memcmp(&operation, &sentinel, sizeof(operation)) == 0);
    Unchanged(runtime, before);
    FreeCapture(before); umi_developer_runtime_destroy(runtime);
}

static void SingleSubmission(const char *mode)
{
    UmiDeveloperRuntime *runtime = Runtime();
    Captured *before = Capture(runtime);
    UmiDeveloperOperationSnapshot value = Operation("single");
    int success = !strcmp(mode, "single-legacy");
    if (!strcmp(mode, "single-id")) memset(value.id, 'x', sizeof(value.id));
    else if (!strcmp(mode, "single-program")) memset(value.program, 'x', sizeof(value.program));
    else if (!strcmp(mode, "single-argument")) { value.argument_count = 1U; memset(value.arguments[0], 'x', sizeof(value.arguments[0])); }
    else if (!strcmp(mode, "single-kind")) value.kind = (UmiDeveloperOperationKind)(UMI_DEVELOPER_OPERATION_AI + 1);
    else if (success) { value.struct_size = 0U; value.api_version = 0U; }
    else assert(!"unknown single submission");
    UmiStatus status = umi_developer_runtime_submit_operation(runtime, &value);
    if (success) {
        assert(status == UMI_STATUS_OK);
        UmiDeveloperOperationSnapshot stored;
        assert(umi_developer_pipeline_find(umi_developer_runtime_pipeline(runtime), "single", &stored) == UMI_STATUS_OK);
        assert(stored.struct_size == sizeof(stored) && stored.api_version == UMI_DEVELOPER_OPERATION_API_VERSION);
    } else { assert(status != UMI_STATUS_OK); Unchanged(runtime, before); }
    FreeCapture(before); umi_developer_runtime_destroy(runtime);
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!strncmp(argv[1], "single-", 7U)) SingleSubmission(argv[1]);
    else if (!strncmp(argv[1], "chain-", 6U)) DirectChain(argv[1]);
    else if (!strcmp(argv[1], "task-plan") || !strcmp(argv[1], "task-missing") ||
             !strcmp(argv[1], "launch-plan") || !strcmp(argv[1], "launch-missing")) Planner(argv[1]);
    else Workflow(argv[1]);
    return 0;
}
