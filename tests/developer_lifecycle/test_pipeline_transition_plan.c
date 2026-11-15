/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_lifecycle/test_pipeline_transition_plan.c
 *
 * PURPOSE:
 *   Verify atomic transition and cascading dependency admission at exact revision boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/pipeline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Access private counters only in this regression translation unit. The real
 * implementation executes every transition; no alternate scheduler is modeled. */
#include "../../src/developer/pipeline.c"

static void Require(int condition, const char *expression, int line)
{
    if (!condition) {
        fprintf(stderr, "line %d: %s\n", line, expression);
        abort();
    }
}
#define REQUIRE(expression) Require((expression) != 0, #expression, __LINE__)

static void Submit(UmiDeveloperPipeline *pipeline, const char *id)
{
    UmiDeveloperOperationSnapshot value;
    REQUIRE(umi_developer_operation_init(&value, id,
        UMI_DEVELOPER_OPERATION_CUSTOM, id) == UMI_STATUS_OK);
    value.max_attempts = 3U;
    REQUIRE(umi_developer_pipeline_submit(pipeline, &value) == UMI_STATUS_OK);
}

static int Is(const char *value, const char *expected)
{
    return strcmp(value, expected) == 0;
}

static int Cascades(const char *action)
{
    return Is(action, "fail") || Is(action, "cancel") || Is(action, "retry") ||
        Is(action, "refresh") || Is(action, "dependency") || Is(action, "execution");
}

static UmiStatus Act(UmiDeveloperPipeline *pipeline, const char *action,
    UmiDeveloperOperationSnapshot *output)
{
    if (Is(action, "start")) return umi_developer_pipeline_start(pipeline, "root");
    if (Is(action, "start-next")) return umi_developer_pipeline_start_next(pipeline, output);
    if (Is(action, "progress")) return umi_developer_pipeline_set_progress(pipeline, "root", 4321U, "Progress");
    if (Is(action, "complete")) return umi_developer_pipeline_complete(pipeline, "root", 0, "Complete");
    if (Is(action, "fail")) return umi_developer_pipeline_fail(pipeline, "root", 7, "Failure");
    if (Is(action, "cancel")) return umi_developer_pipeline_cancel(pipeline, "root", "Cancelled");
    if (Is(action, "retry")) return umi_developer_pipeline_retry(pipeline, "root");
    if (Is(action, "refresh")) return umi_developer_pipeline_refresh(pipeline);
    if (Is(action, "dependency")) return umi_developer_pipeline_add_dependency(pipeline, "middle", "root");
    if (Is(action, "execution")) return umi_developer_pipeline_admit_next_execution(pipeline);
    return UMI_STATUS_INVALID_ARGUMENT;
}

static void Matrix(const char *action, const char *budget)
{
    UmiDeveloperPipeline *pipeline = NULL;
    REQUIRE(umi_developer_pipeline_create(&pipeline) == UMI_STATUS_OK);
    Submit(pipeline, "root"); Submit(pipeline, "middle");
    Submit(pipeline, "leaf"); Submit(pipeline, "unrelated");
    if (!Is(action, "dependency"))
        REQUIRE(umi_developer_pipeline_add_dependency(pipeline, "middle", "root") == UMI_STATUS_OK);
    REQUIRE(umi_developer_pipeline_add_dependency(pipeline, "leaf", "middle") == UMI_STATUS_OK);
    if (Is(action, "progress") || Is(action, "complete") || Is(action, "fail") ||
        Is(action, "retry") || Is(action, "dependency"))
        REQUIRE(umi_developer_pipeline_start(pipeline, "root") == UMI_STATUS_OK);
    if (Is(action, "retry") || Is(action, "dependency"))
        REQUIRE(umi_developer_pipeline_fail(pipeline, "root", 9, "Previous failure") == UMI_STATUS_OK);
    if (Is(action, "refresh")) {
        /* A deliberately stale private state exercises refresh without silently
         * normalizing it through a snapshot before the operation under test. */
        pipeline->operations[0].state = UMI_DEVELOPER_OPERATION_FAILED;
    }
    uint64_t pipeline_cost = Is(action, "execution") ? 3U :
        (Cascades(action) && !Is(action, "refresh") ? 2U : 1U);
    size_t subject = Is(action, "refresh") || Is(action, "dependency") ? 1U : 0U;
    uint64_t subject_cost = Is(action, "execution") ? 2U : 1U;
    int expected_ok = Is(budget, "boundary") || Is(budget, "unrelated") ||
        (Is(budget, "descendant") && !Cascades(action));
    if (Is(budget, "pipeline")) pipeline->revision = UINT64_MAX;
    else if (Is(budget, "subject")) pipeline->operations[subject].revision = UINT64_MAX;
    else if (Is(budget, "descendant")) pipeline->operations[2].revision = UINT64_MAX;
    else if (Is(budget, "unrelated")) pipeline->operations[3].revision = UINT64_MAX;
    else if (Is(budget, "boundary")) {
        pipeline->revision = UINT64_MAX - pipeline_cost;
        pipeline->operations[subject].revision = UINT64_MAX - subject_cost;
        if (Cascades(action)) pipeline->operations[2].revision = UINT64_MAX - 1U;
    } else REQUIRE(0);

    /* Retain the value-copy fixture for review. Byte-preservation assertions
     * capture padding as well as fields so ABI padding cannot affect the check. */
#if 0
    UmiDeveloperPipeline before = *pipeline;
#endif
    UmiDeveloperPipeline before;
    memcpy(&before, pipeline, sizeof(before));
    size_t bytes = pipeline->operation_count * sizeof(*pipeline->operations);
    void *operations = malloc(bytes);
    REQUIRE(operations != NULL);
    memcpy(operations, pipeline->operations, bytes);
    UmiDeveloperDependency edges[UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY];
    memcpy(edges, pipeline->dependencies, pipeline->dependency_count * sizeof(*edges));
    UmiDeveloperOperationSnapshot output, sentinel;
    memset(&output, 0x5a, sizeof(output)); memcpy(&sentinel, &output, sizeof(output));
    UmiStatus status = Act(pipeline, action, &output);
    REQUIRE(status == (expected_ok ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE));
    if (!expected_ok || Is(action, "execution")) {
        REQUIRE(memcmp(&before, pipeline, sizeof(before)) == 0);
        REQUIRE(memcmp(operations, pipeline->operations, bytes) == 0);
        REQUIRE(memcmp(edges, pipeline->dependencies, before.dependency_count * sizeof(*edges)) == 0);
        REQUIRE(memcmp(&output, &sentinel, sizeof(output)) == 0);
    } else {
        REQUIRE(pipeline->revision == before.revision + pipeline_cost);
        if (Is(budget, "boundary")) {
            REQUIRE(pipeline->revision == UINT64_MAX);
            REQUIRE(pipeline->operations[subject].revision == UINT64_MAX);
        }
        if (Is(action, "start") || Is(action, "start-next")) {
            REQUIRE(pipeline->operations[0].state == UMI_DEVELOPER_OPERATION_RUNNING);
            REQUIRE(pipeline->operations[0].attempt_count == 1U);
            if (Is(action, "start-next")) REQUIRE(output.state == UMI_DEVELOPER_OPERATION_RUNNING);
        }
        if (Is(action, "progress")) {
            REQUIRE(pipeline->operations[0].progress_basis_points == 4321U);
            REQUIRE(Is(pipeline->operations[0].summary, "Progress"));
        }
        if (Is(action, "complete")) {
            REQUIRE(pipeline->operations[0].state == UMI_DEVELOPER_OPERATION_SUCCEEDED);
            REQUIRE(pipeline->operations[0].progress_basis_points == 10000U);
            REQUIRE(pipeline->operations[0].has_exit_code != 0);
        }
        if (Cascades(action)) {
            UmiDeveloperOperationState expected = Is(action, "retry") ?
                UMI_DEVELOPER_OPERATION_QUEUED : UMI_DEVELOPER_OPERATION_BLOCKED;
            REQUIRE(pipeline->operations[1].state == expected);
            REQUIRE(pipeline->operations[2].state == expected);
        }
        if (Is(action, "retry")) {
            REQUIRE(pipeline->operations[0].state == UMI_DEVELOPER_OPERATION_QUEUED);
            REQUIRE(pipeline->operations[0].has_exit_code == 0);
            REQUIRE(pipeline->operations[0].summary[0] == '\0');
        }
        if (Is(action, "dependency")) REQUIRE(pipeline->dependency_count == before.dependency_count + 1U);
    }
    free(operations);
    umi_developer_pipeline_destroy(pipeline);
}

static void Special(const char *scenario)
{
    UmiDeveloperPipeline *pipeline = NULL;
    REQUIRE(umi_developer_pipeline_create(&pipeline) == UMI_STATUS_OK);
    Submit(pipeline, "root"); Submit(pipeline, "child");
    REQUIRE(umi_developer_pipeline_add_dependency(pipeline, "child", "root") == UMI_STATUS_OK);
    if (Is(scenario, "no-change")) {
        pipeline->revision = UINT64_MAX;
        pipeline->operations[0].revision = UINT64_MAX;
        pipeline->operations[1].revision = UINT64_MAX;
        REQUIRE(umi_developer_pipeline_refresh(pipeline) == UMI_STATUS_OK);
        REQUIRE(pipeline->revision == UINT64_MAX);
    } else if (Is(scenario, "attempt-boundary")) {
        pipeline->operations[0].attempt_count = UINT32_MAX - 1U;
        pipeline->operations[0].max_attempts = UINT32_MAX;
        REQUIRE(umi_developer_pipeline_start(pipeline, "root") == UMI_STATUS_OK);
        REQUIRE(pipeline->operations[0].attempt_count == UINT32_MAX);
        REQUIRE(umi_developer_pipeline_fail(pipeline, "root", 1, "Failed") == UMI_STATUS_OK);
        REQUIRE(umi_developer_pipeline_retry(pipeline, "root") == UMI_STATUS_INVALID_STATE);
    } else if (Is(scenario, "retry-two-changes")) {
        REQUIRE(umi_developer_pipeline_cancel(pipeline, "root", "Blocked prerequisite") == UMI_STATUS_OK);
        REQUIRE(umi_developer_pipeline_cancel(pipeline, "child", "Child cancelled") == UMI_STATUS_OK);
        pipeline->operations[1].revision = UINT64_MAX - 1U;
        uint64_t before = pipeline->revision;
        REQUIRE(umi_developer_pipeline_retry(pipeline, "child") == UMI_STATUS_INVALID_STATE);
        REQUIRE(pipeline->operations[1].state == UMI_DEVELOPER_OPERATION_CANCELLED);
        REQUIRE(pipeline->operations[1].revision == UINT64_MAX - 1U && pipeline->revision == before);
        pipeline->operations[1].revision = UINT64_MAX - 2U;
        REQUIRE(umi_developer_pipeline_retry(pipeline, "child") == UMI_STATUS_OK);
        REQUIRE(pipeline->operations[1].state == UMI_DEVELOPER_OPERATION_BLOCKED);
        REQUIRE(pipeline->operations[1].revision == UINT64_MAX);
    } else if (Is(scenario, "missing-start")) {
        pipeline->operations[0].state = UMI_DEVELOPER_OPERATION_FAILED;
        uint64_t before = pipeline->revision;
        REQUIRE(umi_developer_pipeline_start(pipeline, "missing") == UMI_STATUS_NOT_FOUND);
        REQUIRE(pipeline->revision == before && pipeline->operations[1].state == UMI_DEVELOPER_OPERATION_QUEUED);
    } else if (Is(scenario, "empty-next")) {
        UmiDeveloperOperationSnapshot output, before;
        memset(&output, 0x5a, sizeof(output)); memcpy(&before, &output, sizeof(output));
        REQUIRE(umi_developer_pipeline_cancel(pipeline, "root", "Closed") == UMI_STATUS_OK);
        uint64_t revision = pipeline->revision;
        REQUIRE(umi_developer_pipeline_start_next(pipeline, &output) == UMI_STATUS_NOT_FOUND);
        REQUIRE(umi_developer_pipeline_admit_next_execution(pipeline) == UMI_STATUS_NOT_FOUND);
        REQUIRE(pipeline->revision == revision && memcmp(&output, &before, sizeof(output)) == 0);
    } else REQUIRE(0);
    umi_developer_pipeline_destroy(pipeline);
}

static void Deep(int exhausted)
{
    UmiDeveloperPipeline *pipeline = NULL;
    REQUIRE(umi_developer_pipeline_create(&pipeline) == UMI_STATUS_OK);
    char current[32], dependency[32];
    for (unsigned i = 0U; i < UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY; ++i) {
        (void)snprintf(current, sizeof(current), "node.%u", i); Submit(pipeline, current);
    }
    for (unsigned i = 0U; i + 1U < UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY; ++i) {
        (void)snprintf(current, sizeof(current), "node.%u", i);
        (void)snprintf(dependency, sizeof(dependency), "node.%u", i + 1U);
        REQUIRE(umi_developer_pipeline_add_dependency(pipeline, current, dependency) == UMI_STATUS_OK);
    }
    if (exhausted) pipeline->operations[0].revision = UINT64_MAX;
    size_t bytes = pipeline->operation_count * sizeof(*pipeline->operations);
    void *before = malloc(bytes); REQUIRE(before != NULL); memcpy(before, pipeline->operations, bytes);
    uint64_t revision = pipeline->revision;
    REQUIRE(umi_developer_pipeline_cancel(pipeline, "node.255", "Root cancelled") ==
        (exhausted ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK));
    if (exhausted) {
        REQUIRE(memcmp(before, pipeline->operations, bytes) == 0 && pipeline->revision == revision);
    } else {
        REQUIRE(pipeline->revision == revision + 2U);
        for (size_t i = 0U; i < 255U; ++i)
            REQUIRE(pipeline->operations[i].state == UMI_DEVELOPER_OPERATION_BLOCKED);
    }
    free(before); umi_developer_pipeline_destroy(pipeline);
}

int main(int argc, char **argv)
{
    if (argc == 3) Matrix(argv[1], argv[2]);
    else if (argc == 2 && Is(argv[1], "deep")) Deep(0);
    else if (argc == 2 && Is(argv[1], "deep-refusal")) Deep(1);
    else if (argc == 2) Special(argv[1]);
    else return 2;
    return 0;
}
