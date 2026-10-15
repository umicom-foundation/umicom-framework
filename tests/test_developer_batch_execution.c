/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_batch_execution.c
 *
 * PURPOSE:
 *   Verify dependency-ready operations can execute as one bounded batch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/batch_execution.h"
#include "value_archive/construction_cases.h"

#include <assert.h>
#include <string.h>

/*
 * Perform fake through the module contract so client applications do not duplicate its
 * policy.
 */
static UmiStatus fake_execute(
    void *user_data,
    const UmiDeveloperOperationSnapshot *operation,
    UmiDeveloperExecutionResult *out_result)
{
    size_t *calls = (size_t *)user_data;
    (void)operation;
    *calls += 1U;
    memset(out_result, 0, sizeof(*out_result));
    out_result->struct_size = (uint32_t)sizeof(*out_result);
    out_result->api_version = UMI_DEVELOPER_EXECUTION_API_VERSION;
    out_result->launched = 1;
    out_result->exit_code = 0;
    return UMI_STATUS_OK;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted operation value. */
static int UmiDeveloperOperationSnapshotConstructionCases(void)
{
    UmiDeveloperOperationSnapshot *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_developer_operation_init(value, "one", UMI_DEVELOPER_OPERATION_BUILD, "One") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_developer_operation_init(value, oversized, UMI_DEVELOPER_OPERATION_BUILD, "One") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_developer_operation_init(value, value->id, UMI_DEVELOPER_OPERATION_BUILD, "One") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->id, "one") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->title) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_developer_operation_init(value, "one", UMI_DEVELOPER_OPERATION_BUILD, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_developer_operation_init(value, "one", UMI_DEVELOPER_OPERATION_BUILD, value->title) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->title, "One") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiDeveloperOperationSnapshotConstructionCases() != 0) return 1;

    UmiDeveloperRuntime *runtime = NULL;
    UmiDeveloperOperationSnapshot first, second;
    UmiDeveloperExecutor executor;
    UmiDeveloperBatchExecutionRequest request = {0};
    UmiDeveloperBatchExecutionSnapshot batch;
    size_t calls = 0U;

    assert(umi_developer_runtime_create_owned(&runtime) == UMI_STATUS_OK);
    assert(umi_developer_operation_init(&first, "one", UMI_DEVELOPER_OPERATION_BUILD, "One") == UMI_STATUS_OK);
    assert(umi_developer_operation_set_program(&first, "one", NULL) == UMI_STATUS_OK);
    assert(umi_developer_operation_init(&second, "two", UMI_DEVELOPER_OPERATION_TEST, "Two") == UMI_STATUS_OK);
    assert(umi_developer_operation_set_program(&second, "two", NULL) == UMI_STATUS_OK);
    assert(umi_developer_runtime_submit_operation(runtime, &first) == UMI_STATUS_OK);
    assert(umi_developer_runtime_submit_operation(runtime, &second) == UMI_STATUS_OK);
    assert(umi_developer_runtime_add_dependency(runtime, "two", "one") == UMI_STATUS_OK);

    umi_developer_executor_init(&executor, fake_execute, &calls);
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_BATCH_EXECUTION_API_VERSION;
    assert(umi_developer_batch_execute(runtime, &executor, &request, &batch) == UMI_STATUS_OK);
    assert(calls == 2U);
    assert(batch.succeeded_count == 2U);
    assert(batch.pipeline.succeeded_count == 2U);

    umi_developer_runtime_destroy(runtime);
    return 0;
}
