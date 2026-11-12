/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_lifecycle/test_runtime_transition_budget.c
 *
 * PURPOSE:
 *   Verify lifecycle state is preserved when event identities are exhausted,
 *   including refusal before a synchronous executor can cause external effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../support/assertions.h"
#include "umicom/developer/developer.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static const char *injected = "none";
static UmiStatus TransitionJournalSnapshot(const UmiDeveloperJournal *journal,
    UmiDeveloperJournalSnapshot *out);
/* Inject the observed budget, retaining the real runtime and queue logic. */
#define umi_developer_journal_snapshot TransitionJournalSnapshot
#include "../../src/developer/runtime.c"
#undef umi_developer_journal_snapshot

static UmiStatus TransitionJournalSnapshot(const UmiDeveloperJournal *journal,
    UmiDeveloperJournalSnapshot *out)
{
    UmiStatus status = umi_developer_journal_snapshot(journal, out);
    if (status != UMI_STATUS_OK) return status;
    if (!strcmp(injected, "sequence")) out->next_sequence = UINT64_MAX;
    if (!strcmp(injected, "journal")) out->revision = UINT64_MAX;
    if (!strcmp(injected, "dropped")) {
        out->entry_count = UMI_DEVELOPER_JOURNAL_CAPACITY;
        out->dropped_count = UINT64_MAX;
    }
    return status;
}

static UmiStatus TransitionExecute(void *user_data,
    const UmiDeveloperOperationSnapshot *operation,
    UmiDeveloperExecutionResult *out)
{
    unsigned *calls = user_data;
    ++*calls;
    assert(operation->state == UMI_DEVELOPER_OPERATION_RUNNING);
    out->exit_code = 0;
    strcpy(out->output, "Fixture completion; no process was launched.");
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *action = argv[1], *fault_name = argv[2];
    const char *actions[] = {"context", "dependency", "start", "progress",
                            "complete", "fail", "cancel", "retry", "execute"};
    unsigned selected = 0U;
    while (selected < sizeof(actions) / sizeof(actions[0]) &&
           strcmp(action, actions[selected])) ++selected;
    if (selected == sizeof(actions) / sizeof(actions[0])) return 2;
    if (strcmp(fault_name, "runtime") && strcmp(fault_name, "sequence") &&
        strcmp(fault_name, "journal") && strcmp(fault_name, "dropped") &&
        strcmp(fault_name, "boundary") && strcmp(fault_name, "one-event")) return 2;
    if (!strcmp(fault_name, "one-event") && selected != 8U) return 2;

    UmiDeveloperRuntime *runtime = NULL;
    assert(umi_developer_runtime_create_owned(&runtime) == UMI_STATUS_OK);
    UmiDeveloperOperationSnapshot *value = calloc(1U, sizeof(*value));
    assert(value != NULL);
    assert(umi_developer_operation_init(value, "subject", UMI_DEVELOPER_OPERATION_CUSTOM,
        "Subject") == UMI_STATUS_OK);
    value->max_attempts = 2U;
    assert(umi_developer_runtime_submit_operation(runtime, value) == UMI_STATUS_OK);
    if (selected >= 3U && selected <= 7U)
        assert(umi_developer_runtime_start_operation(runtime, "subject") == UMI_STATUS_OK);
    if (selected == 7U)
        assert(umi_developer_runtime_fail_operation(runtime, "subject", 1, "Retry fixture") == UMI_STATUS_OK);
    if (selected == 1U) {
        assert(umi_developer_operation_init(value, "dependency", UMI_DEVELOPER_OPERATION_CUSTOM,
            "Dependency") == UMI_STATUS_OK);
        assert(umi_developer_runtime_submit_operation(runtime, value) == UMI_STATUS_OK);
    }
    UmiDeveloperContextSnapshot context;
    assert(umi_developer_context_snapshot(runtime->context, &context) == UMI_STATUS_OK);
    strcpy(context.project_id, "changed-project");
    uint64_t event_count = selected == 8U ? 2U : 1U;
    injected = fault_name;
    if (!strcmp(fault_name, "runtime")) runtime->revision = UINT64_MAX;
    if (!strcmp(fault_name, "boundary")) runtime->revision = UINT64_MAX - event_count;
    if (!strcmp(fault_name, "one-event")) runtime->revision = UINT64_MAX - 1U;

    UmiDeveloperPipelineSnapshot before_pipeline, after_pipeline;
    UmiDeveloperJournalSnapshot before_journal, after_journal;
    UmiDeveloperContextSnapshot before_context, after_context;
    UmiDeveloperOperationSnapshot *before_value = calloc(1U, sizeof(*before_value));
    assert(before_value != NULL);
    assert(umi_developer_pipeline_snapshot(runtime->pipeline, &before_pipeline) == UMI_STATUS_OK);
    assert(umi_developer_journal_snapshot(runtime->journal, &before_journal) == UMI_STATUS_OK);
    assert(umi_developer_context_snapshot(runtime->context, &before_context) == UMI_STATUS_OK);
    assert(umi_developer_pipeline_find(runtime->pipeline, "subject", before_value) == UMI_STATUS_OK);
    uint64_t before_revision = runtime->revision;
    unsigned calls = 0U;
    UmiDeveloperExecutor executor;
    umi_developer_executor_init(&executor, TransitionExecute, &calls);
    UmiDeveloperExecutionResult result, sentinel;
    memset(&result, 0x5a, sizeof(result)); memcpy(&sentinel, &result, sizeof(result));
    UmiStatus status;
    switch (selected) {
        case 0: status = umi_developer_runtime_set_context(runtime, &context); break;
        case 1: status = umi_developer_runtime_add_dependency(runtime, "subject", "dependency"); break;
        case 2: status = umi_developer_runtime_start_operation(runtime, "subject"); break;
        case 3: status = umi_developer_runtime_set_progress(runtime, "subject", 5000U, "Progress"); break;
        case 4: status = umi_developer_runtime_complete_operation(runtime, "subject", 0, "Complete"); break;
        case 5: status = umi_developer_runtime_fail_operation(runtime, "subject", 1, "Fail"); break;
        case 6: status = umi_developer_runtime_cancel_operation(runtime, "subject", "Cancel"); break;
        case 7: status = umi_developer_runtime_retry_operation(runtime, "subject"); break;
        case 8: status = umi_developer_runtime_execute_next(runtime, &executor, NULL, &result); break;
        default: return 2;
    }
    assert(umi_developer_pipeline_snapshot(runtime->pipeline, &after_pipeline) == UMI_STATUS_OK);
    assert(umi_developer_journal_snapshot(runtime->journal, &after_journal) == UMI_STATUS_OK);
    assert(umi_developer_context_snapshot(runtime->context, &after_context) == UMI_STATUS_OK);
    assert(umi_developer_pipeline_find(runtime->pipeline, "subject", value) == UMI_STATUS_OK);
    if (!strcmp(fault_name, "boundary")) {
        assert(status == UMI_STATUS_OK && runtime->revision == UINT64_MAX);
        assert(after_journal.entry_count == before_journal.entry_count + event_count);
        assert(after_journal.next_sequence == before_journal.next_sequence + event_count);
        assert(calls == (selected == 8U ? 1U : 0U));
        if (selected == 8U) assert(value->state == UMI_DEVELOPER_OPERATION_SUCCEEDED && result.exit_code == 0);
    } else {
        assert(status == UMI_STATUS_INVALID_STATE && runtime->revision == before_revision);
        assert(calls == 0U && memcmp(&result, &sentinel, sizeof(result)) == 0);
        assert(after_pipeline.revision == before_pipeline.revision);
        assert(after_pipeline.dependency_count == before_pipeline.dependency_count);
        assert(after_journal.revision == before_journal.revision);
        assert(after_journal.next_sequence == before_journal.next_sequence);
        assert(after_journal.dropped_count == before_journal.dropped_count);
        assert(after_context.revision == before_context.revision);
        assert(strcmp(after_context.project_id, before_context.project_id) == 0);
        assert(value->revision == before_value->revision && value->state == before_value->state);
        assert(value->progress_basis_points == before_value->progress_basis_points);
        assert(value->attempt_count == before_value->attempt_count);
    }
    free(before_value); free(value); umi_developer_runtime_destroy(runtime);
    return 0;
}
