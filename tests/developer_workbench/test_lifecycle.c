/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_lifecycle.c
 *
 * PURPOSE:
 *   Verify the lifecycle exposes cooperative cancellation without invoking an
 *   external tool.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_workbench/lifecycle.h"

int main(void)
{
    UmiToolchainProfile profile;
    UmiDeveloperWorkbenchConfigurationRegistry *configurations = NULL;
    UmiDeveloperWorkbenchOperationHistory *history = NULL;
    UmiDeveloperWorkbenchLifecycle *lifecycle = NULL;

    umi_toolchain_profile_init(&profile);

    assert(umi_developer_workbench_configuration_registry_create(
        &configurations) == UMI_STATUS_OK);
    assert(umi_developer_workbench_operation_history_create(&history) ==
           UMI_STATUS_OK);
    assert(umi_developer_workbench_lifecycle_create(
        &profile,
        NULL,
        configurations,
        history,
        &lifecycle) == UMI_STATUS_OK);

    assert(!umi_developer_workbench_lifecycle_is_cancel_requested(lifecycle));
    assert(umi_developer_workbench_lifecycle_execute(
        lifecycle,
        "build.cancel",
        UMI_DEVELOPER_WORKBENCH_ACTION_CANCEL,
        NULL) == UMI_STATUS_OK);
    assert(umi_developer_workbench_lifecycle_is_cancel_requested(lifecycle));

    umi_developer_workbench_lifecycle_destroy(lifecycle);
    umi_developer_workbench_operation_history_destroy(history);
    umi_developer_workbench_configuration_registry_destroy(configurations);
    return 0;
}
