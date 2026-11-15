/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/pipeline_lifecycle/main.c
 *
 * PURPOSE:
 *   Demonstrate shared workflow execution, dependency blocking and retry through an installed SDK with a synthetic executor.
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

typedef struct TeachingExecutor {
    const char *scenario;
    unsigned calls;
    int refusal_observed;
} TeachingExecutor;

/* The application supplies only a worker outcome. Framework owns ready order,
 * state changes, dependency blocking, retry and event publication. This example
 * does not start a compiler, shell, installer or network operation. */
static UmiStatus Observe(void *data, const UmiDeveloperOperationSnapshot *operation,
    UmiDeveloperExecutionResult *result)
{
    TeachingExecutor *executor = data;
    ++executor->calls;
    (void)snprintf(result->output, sizeof(result->output),
        "Synthetic outcome for %s", operation->id);
    if (strcmp(operation->id, "compile") == 0 && executor->refusal_observed == 0) {
        if (strcmp(executor->scenario, "failure") == 0 ||
            strcmp(executor->scenario, "retry") == 0) {
            executor->refusal_observed = 1;
            result->exit_code = 7;
        } else if (strcmp(executor->scenario, "cancel") == 0) {
            executor->refusal_observed = 1;
            result->cancelled = 1;
            return UMI_STATUS_CANCELLED;
        }
    }
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    if (argc != 2 || (strcmp(argv[1], "success") && strcmp(argv[1], "failure") &&
        strcmp(argv[1], "cancel") && strcmp(argv[1], "retry"))) {
        fputs("Usage: pipeline-lifecycle success|failure|cancel|retry\n", stderr);
        return 2;
    }
    UmiDeveloperRuntime *runtime = NULL;
    UmiDeveloperOperationSnapshot *stages = calloc(3U, sizeof(*stages));
    if (stages == NULL) return 1;
    UmiStatus status = umi_developer_runtime_create_owned(&runtime);
    const char *names[] = {"compile", "test", "package"};
    for (size_t i = 0U; i < 3U && status == UMI_STATUS_OK; ++i) {
        status = umi_developer_operation_init(&stages[i], names[i],
            UMI_DEVELOPER_OPERATION_CUSTOM, names[i]);
        stages[i].max_attempts = 2U;
    }
    if (status == UMI_STATUS_OK)
        status = umi_developer_runtime_submit_chain(runtime, stages, 3U);
    TeachingExecutor teaching = {argv[1], 0U, 0};
    UmiDeveloperExecutor executor;
    umi_developer_executor_init(&executor, Observe, &teaching);
    int retried = 0;
    while (status == UMI_STATUS_OK) {
        UmiDeveloperExecutionResult result;
        UmiDeveloperOperationSnapshot operation;
        status = umi_developer_runtime_execute_next(runtime, &executor, &operation, &result);
        if (status == UMI_STATUS_NOT_FOUND) { status = UMI_STATUS_OK; break; }
        if (status != UMI_STATUS_OK) break;
        printf("%s: state=%d attempt=%u revision=%llu\n", operation.id,
            (int)operation.state, operation.attempt_count, (unsigned long long)operation.revision);
        if (strcmp(argv[1], "retry") == 0 && !retried &&
            operation.state == UMI_DEVELOPER_OPERATION_FAILED) {
            status = umi_developer_runtime_retry_operation(runtime, operation.id);
            retried = status == UMI_STATUS_OK;
        }
    }
    if (status == UMI_STATUS_OK) {
        UmiDeveloperPipelineSnapshot snapshot;
        status = umi_developer_pipeline_snapshot(umi_developer_runtime_pipeline(runtime), &snapshot);
        if (status == UMI_STATUS_OK) {
            int success = strcmp(argv[1], "success") == 0 || strcmp(argv[1], "retry") == 0;
            unsigned expected_calls = strcmp(argv[1], "retry") == 0 ? 4U : (success ? 3U : 1U);
            if (teaching.calls != expected_calls ||
                snapshot.succeeded_count != (success ? 3U : 0U) ||
                snapshot.blocked_count != (success ? 0U : 2U) ||
                snapshot.failed_count != (strcmp(argv[1], "failure") == 0 ? 1U : 0U) ||
                snapshot.cancelled_count != (strcmp(argv[1], "cancel") == 0 ? 1U : 0U))
                status = UMI_STATUS_INTERNAL_ERROR;
            printf("calls=%u succeeded=%zu failed=%zu cancelled=%zu blocked=%zu\n",
                teaching.calls, snapshot.succeeded_count, snapshot.failed_count,
                snapshot.cancelled_count, snapshot.blocked_count);
        }
    }
    if (status != UMI_STATUS_OK)
        fprintf(stderr, "Workflow model refused: %s\n", umi_status_text(status));
    else puts("Model observations complete. No application artifact or deployment was produced.");
    umi_developer_runtime_destroy(runtime);
    free(stages);
    return status == UMI_STATUS_OK ? 0 : 1;
}
