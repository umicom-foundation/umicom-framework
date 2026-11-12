/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_lifecycle/test_runtime_publication_budget.c
 *
 * PURPOSE:
 *   Verify runtime publication refuses exhausted event identities before queue or journal mutation.
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

static const char *fault;
static UmiStatus BudgetJournalSnapshot(const UmiDeveloperJournal *journal,
    UmiDeveloperJournalSnapshot *out);
/* Inject only the observed identity budget, not an alternative queue or event
 * writer. Runtime revision exhaustion uses the actual owner field below. */
#define umi_developer_journal_snapshot BudgetJournalSnapshot
#include "../../src/developer/runtime.c"
#undef umi_developer_journal_snapshot

static UmiStatus BudgetJournalSnapshot(const UmiDeveloperJournal *journal,
    UmiDeveloperJournalSnapshot *out)
{
    UmiStatus status = umi_developer_journal_snapshot(journal, out);
    if (status != UMI_STATUS_OK) return status;
    if (!strcmp(fault, "sequence")) out->next_sequence = UINT64_MAX;
    else if (!strcmp(fault, "journal-revision")) out->revision = UINT64_MAX;
    else if (!strcmp(fault, "dropped")) {
        out->entry_count = UMI_DEVELOPER_JOURNAL_CAPACITY;
        out->dropped_count = UINT64_MAX;
    }
    return status;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    int cmake = !strncmp(argv[1], "cmake-", 6U);
    int single = !strncmp(argv[1], "single-", 7U);
    if (!cmake && !single && strncmp(argv[1], "chain-", 6U)) return 2;
    /* The former fixture activated injection before seeding existing work.
     * Single-operation admission now checks that budget too, so activate the
     * requested fault only after the running fixture has been established. */
#if 0
    fault = argv[1] + 6U;
#endif
    const char *requested_fault = argv[1] + (single ? 7U : 6U);
    fault = requested_fault;
    if (strcmp(fault, "runtime") && strcmp(fault, "sequence") &&
        strcmp(fault, "journal-revision") && strcmp(fault, "dropped") &&
        strcmp(fault, "boundary")) return 2;
    fault = "none";
    UmiDeveloperRuntime *runtime = NULL;
    assert(umi_developer_runtime_create_owned(&runtime) == UMI_STATUS_OK);
    UmiDeveloperOperationSnapshot *values = calloc(2U, sizeof(*values));
    assert(values != NULL);
    assert(umi_developer_operation_init(&values[0], "existing", UMI_DEVELOPER_OPERATION_CUSTOM, "Existing") == UMI_STATUS_OK);
    assert(umi_developer_runtime_submit_operation(runtime, &values[0]) == UMI_STATUS_OK);
    assert(umi_developer_runtime_start_operation(runtime, "existing") == UMI_STATUS_OK);
    assert(umi_developer_operation_init(&values[0], "first", UMI_DEVELOPER_OPERATION_CUSTOM, "First") == UMI_STATUS_OK);
    assert(umi_developer_operation_init(&values[1], "second", UMI_DEVELOPER_OPERATION_CUSTOM, "Second") == UMI_STATUS_OK);
    fault = requested_fault;
    uint64_t events = single ? 1U : (cmake ? 2U : 3U);
    if (!strcmp(fault, "runtime")) runtime->revision = UINT64_MAX - events + 1U;
    if (!strcmp(fault, "boundary")) runtime->revision = UINT64_MAX - events;
    uint64_t revision = runtime->revision;
    UmiDeveloperPipelineSnapshot before_pipeline, after_pipeline;
    UmiDeveloperJournalSnapshot before_journal, after_journal;
    UmiDeveloperOperationSnapshot before_operation, after_operation;
    assert(umi_developer_pipeline_snapshot(runtime->pipeline, &before_pipeline) == UMI_STATUS_OK);
    assert(umi_developer_journal_snapshot(runtime->journal, &before_journal) == UMI_STATUS_OK);
    assert(umi_developer_pipeline_at(runtime->pipeline, 0U, &before_operation) == UMI_STATUS_OK);
    UmiDeveloperCMakePlanRequest request = {0};
    request.plan_id = "cmake"; request.source_directory = "source"; request.build_directory = "build";
    request.include_build = request.include_tests = 1;
    UmiDeveloperCMakePlanSnapshot output, sentinel;
    memset(&output, 0x5a, sizeof(output)); memcpy(&sentinel, &output, sizeof(output));
    /* Preserve the original publication modes while adding single-operation
     * admission to the same actual-runtime fault injector. */
#if 0
    UmiStatus status = cmake ? umi_developer_runtime_submit_cmake_plan(runtime, &request, &output) :
        umi_developer_runtime_submit_chain(runtime, values, 2U);
#endif
    UmiStatus status = single ? umi_developer_runtime_submit_operation(runtime, &values[0]) :
        (cmake ? umi_developer_runtime_submit_cmake_plan(runtime, &request, &output) :
         umi_developer_runtime_submit_chain(runtime, values, 2U));
    assert(umi_developer_pipeline_snapshot(runtime->pipeline, &after_pipeline) == UMI_STATUS_OK);
    assert(umi_developer_journal_snapshot(runtime->journal, &after_journal) == UMI_STATUS_OK);
    assert(umi_developer_pipeline_at(runtime->pipeline, 0U, &after_operation) == UMI_STATUS_OK);
    assert(memcmp(&before_operation, &after_operation, sizeof(before_operation)) == 0);
    if (!strcmp(fault, "boundary")) {
        assert(status == UMI_STATUS_OK && runtime->revision == UINT64_MAX);
        /* The existing chain/CMake cases still add two operations. */
#if 0
        assert(after_pipeline.operation_count == before_pipeline.operation_count + 2U);
#endif
        assert(after_pipeline.operation_count == before_pipeline.operation_count + (single ? 1U : 2U));
        assert(after_journal.entry_count == before_journal.entry_count + events);
        if (cmake) assert(output.operation_count == 2U);
    } else {
        assert(status == UMI_STATUS_INVALID_STATE);
        assert(runtime->revision == revision);
        assert(memcmp(&before_pipeline, &after_pipeline, sizeof(before_pipeline)) == 0);
        assert(memcmp(&before_journal, &after_journal, sizeof(before_journal)) == 0);
        assert(memcmp(&output, &sentinel, sizeof(output)) == 0);
    }
    free(values); umi_developer_runtime_destroy(runtime);
    return 0;
}
