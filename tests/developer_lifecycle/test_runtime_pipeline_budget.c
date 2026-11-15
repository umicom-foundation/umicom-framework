/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_lifecycle/test_runtime_pipeline_budget.c
 *
 * PURPOSE:
 *   Verify shared model budget admission precedes synchronous executor effects and runtime event publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/developer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Include the actual model and runtime for private boundary injection. Other
 * services stay linked normally; no process backend is called by this fixture. */
#include "../../src/developer/pipeline.c"
#include "../../src/developer/runtime.c"

static void Require(int condition, const char *expression, int line)
{
    if (!condition) { fprintf(stderr, "line %d: %s\n", line, expression); abort(); }
}
#define REQUIRE(expression) Require((expression) != 0, #expression, __LINE__)
static int Is(const char *value, const char *expected) { return strcmp(value, expected) == 0; }

typedef struct ExecutorState { unsigned calls; const char *outcome; } ExecutorState;
static UmiStatus Execute(void *data, const UmiDeveloperOperationSnapshot *operation,
    UmiDeveloperExecutionResult *result)
{
    ExecutorState *state = data;
    ++state->calls;
    REQUIRE(operation->state == UMI_DEVELOPER_OPERATION_RUNNING);
    strcpy(result->output, "Synthetic execution; no process or artifact.");
    if (Is(state->outcome, "failure")) result->exit_code = 7;
    if (Is(state->outcome, "cancel")) { result->cancelled = 1; return UMI_STATUS_CANCELLED; }
    if (Is(state->outcome, "timeout")) result->timed_out = 1;
    if (Is(state->outcome, "error")) return UMI_STATUS_INTERNAL_ERROR;
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *budget = argv[1], *outcome = argv[2];
    UmiDeveloperRuntime *runtime = NULL;
    REQUIRE(umi_developer_runtime_create_owned(&runtime) == UMI_STATUS_OK);
    UmiDeveloperOperationSnapshot *values = calloc(4U, sizeof(*values));
    REQUIRE(values != NULL);
    const char *ids[] = {"root", "middle", "leaf", "unrelated"};
    for (size_t i = 0U; i < 4U; ++i) {
        REQUIRE(umi_developer_operation_init(&values[i], ids[i],
            UMI_DEVELOPER_OPERATION_CUSTOM, ids[i]) == UMI_STATUS_OK);
        values[i].max_attempts = 2U;
        REQUIRE(umi_developer_runtime_submit_operation(runtime, &values[i]) == UMI_STATUS_OK);
    }
    REQUIRE(umi_developer_runtime_add_dependency(runtime, "middle", "root") == UMI_STATUS_OK);
    REQUIRE(umi_developer_runtime_add_dependency(runtime, "leaf", "middle") == UMI_STATUS_OK);
    UmiDeveloperPipeline *pipeline = runtime->pipeline;
    int accepted = Is(budget, "exact") || Is(budget, "unrelated") || Is(budget, "normal");
    if (Is(budget, "pipeline-zero")) pipeline->revision = UINT64_MAX;
    else if (Is(budget, "pipeline-one")) pipeline->revision = UINT64_MAX - 1U;
    else if (Is(budget, "pipeline-two")) pipeline->revision = UINT64_MAX - 2U;
    else if (Is(budget, "operation-one")) pipeline->operations[0].revision = UINT64_MAX - 1U;
    else if (Is(budget, "descendant")) pipeline->operations[2].revision = UINT64_MAX;
    else if (Is(budget, "refresh-revisit")) {
        pipeline->operations[1].state = UMI_DEVELOPER_OPERATION_BLOCKED;
        pipeline->operations[1].revision = UINT64_MAX - 1U;
    } else if (Is(budget, "exact")) {
        pipeline->revision = UINT64_MAX - 3U;
        pipeline->operations[0].revision = UINT64_MAX - 2U;
        pipeline->operations[1].revision = UINT64_MAX - 1U;
        pipeline->operations[2].revision = UINT64_MAX - 1U;
    } else if (Is(budget, "unrelated")) pipeline->operations[3].revision = UINT64_MAX;
    else if (!Is(budget, "normal") && !Is(budget, "next-start")) return 2;
    if (Is(budget, "next-start")) {
        pipeline->operations[1].state = UMI_DEVELOPER_OPERATION_BLOCKED;
        pipeline->operations[0].revision = UINT64_MAX;
    }
    /* Retain the value-copy fixture for review. Byte-preservation assertions
     * capture padding as well as fields so ABI padding cannot affect the check. */
#if 0
    UmiDeveloperPipeline before = *pipeline;
#endif
    UmiDeveloperPipeline before;
    memcpy(&before, pipeline, sizeof(before));
    memcpy(values, pipeline->operations, 4U * sizeof(*values));
    UmiDeveloperJournalSnapshot journal_before, journal_after;
    REQUIRE(umi_developer_journal_snapshot(runtime->journal, &journal_before) == UMI_STATUS_OK);
    uint64_t runtime_revision = runtime->revision;
    UmiDeveloperOperationSnapshot operation, sentinel_operation;
    UmiDeveloperExecutionResult result, sentinel_result;
    memset(&operation, 0x5a, sizeof(operation)); memcpy(&sentinel_operation, &operation, sizeof(operation));
    memset(&result, 0x5a, sizeof(result)); memcpy(&sentinel_result, &result, sizeof(result));
    ExecutorState state = {0U, outcome};
    UmiDeveloperExecutor executor;
    umi_developer_executor_init(&executor, Execute, &state);
    UmiStatus status = Is(budget, "next-start") ?
        umi_developer_runtime_start_next(runtime, &operation) :
        umi_developer_runtime_execute_next(runtime, &executor, &operation, &result);
    REQUIRE(umi_developer_journal_snapshot(runtime->journal, &journal_after) == UMI_STATUS_OK);
    if (!accepted) {
        REQUIRE(status == UMI_STATUS_INVALID_STATE && state.calls == 0U);
        REQUIRE(memcmp(&before, pipeline, sizeof(before)) == 0);
        REQUIRE(memcmp(values, pipeline->operations, 4U * sizeof(*values)) == 0);
        REQUIRE(runtime->revision == runtime_revision);
        REQUIRE(journal_after.revision == journal_before.revision);
        REQUIRE(journal_after.entry_count == journal_before.entry_count);
        REQUIRE(journal_after.next_sequence == journal_before.next_sequence);
        REQUIRE(memcmp(&operation, &sentinel_operation, sizeof(operation)) == 0);
        REQUIRE(memcmp(&result, &sentinel_result, sizeof(result)) == 0);
    } else {
        REQUIRE(status == UMI_STATUS_OK && state.calls == 1U);
        REQUIRE(runtime->revision == runtime_revision + 2U);
        REQUIRE(journal_after.entry_count == journal_before.entry_count + 2U);
        REQUIRE(pipeline->operations[0].attempt_count == 1U);
        REQUIRE(pipeline->operations[0].revision == values[0].revision + 2U);
        UmiDeveloperOperationState terminal = Is(outcome, "success") ?
            UMI_DEVELOPER_OPERATION_SUCCEEDED : (Is(outcome, "cancel") ?
            UMI_DEVELOPER_OPERATION_CANCELLED : UMI_DEVELOPER_OPERATION_FAILED);
        REQUIRE(operation.state == terminal && pipeline->operations[0].state == terminal);
        REQUIRE(pipeline->revision == before.revision + (Is(outcome, "success") ? 2U : 3U));
        for (size_t i = 1U; i < 3U; ++i)
            REQUIRE(pipeline->operations[i].state == (Is(outcome, "success") ?
                UMI_DEVELOPER_OPERATION_QUEUED : UMI_DEVELOPER_OPERATION_BLOCKED));
        if (Is(budget, "exact")) REQUIRE(pipeline->operations[0].revision == UINT64_MAX);
    }
    free(values);
    umi_developer_runtime_destroy(runtime);
    return 0;
}
