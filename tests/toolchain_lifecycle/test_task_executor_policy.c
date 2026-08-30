/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_task_executor_policy.c
 *
 * PURPOSE:
 *   Verify trust-required tasks cannot run from an untrusted workspace and are
 *   permitted after an explicit Framework trust decision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/toolchain/task_executor.h"

int main(void)
{
    UmiToolchainProfile profile;
    UmiBuildRequest request;
    UmiToolchainTaskExecutor executor;
    UmiWorkspaceTrustStore *trust = NULL;
    UmiBuildTaskSnapshot task;

    umi_toolchain_profile_init(&profile);
    umi_build_request_init(&request);
    request.source_root = "C:/work/example";

    assert(umi_toolchain_task_executor_init(
        &executor, &profile, NULL, &request) == UMI_STATUS_OK);

    (void)memset(&task, 0, sizeof(task));
    task.kind = UMI_BUILD_TASK_COMMAND;
    task.flags = UMI_BUILD_TASK_ENABLED |
                 UMI_BUILD_TASK_REQUIRES_WORKSPACE_TRUST;

    assert(umi_toolchain_task_executor_authorise(
        &executor, &task) == UMI_STATUS_PERMISSION_DENIED);

    assert(umi_workspace_trust_store_create(&trust) == UMI_STATUS_OK);
    assert(umi_toolchain_task_executor_set_workspace_trust(
        &executor, trust, "C:/work/example") == UMI_STATUS_OK);

    assert(umi_workspace_trust_store_set(
        trust, "C:/work/example", UMI_WORKSPACE_RESTRICTED,
        "test", 1U) == UMI_STATUS_OK);
    assert(umi_toolchain_task_executor_authorise(
        &executor, &task) == UMI_STATUS_PERMISSION_DENIED);

    assert(umi_workspace_trust_store_set(
        trust, "C:/work/example", UMI_WORKSPACE_TRUSTED,
        "test", 2U) == UMI_STATUS_OK);
    assert(umi_toolchain_task_executor_authorise(
        &executor, &task) == UMI_STATUS_OK);

    task.flags = UMI_BUILD_TASK_ENABLED;
    assert(umi_toolchain_task_executor_authorise(
        &executor, &task) == UMI_STATUS_OK);

    umi_workspace_trust_store_destroy(trust);
    return 0;
}
