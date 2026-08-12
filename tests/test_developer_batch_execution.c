/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_batch_execution.c
 *
 * PURPOSE:
 *   Verify dependency-ready operations can execute as one bounded batch.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/batch_execution.h"

#include <assert.h>
#include <string.h>

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

int main(void)
{
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
