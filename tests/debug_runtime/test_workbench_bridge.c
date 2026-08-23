/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_workbench_bridge.c
 *
 * PURPOSE:
 *   Verify existing Developer Workbench Debug commands become context-aware DAP
 *   actions without launching an external adapter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/debug_runtime/workbench_bridge.h"

int main(void)
{
    UmiDebugRuntimePlatform *runtime = NULL;
    UmiToolchainProfile toolchain;
    UmiDeveloperWorkbenchBindings bindings;
    UmiDeveloperWorkbench *workbench = NULL;
    UmiDebugRuntimeWorkbenchBridge *bridge = NULL;
    UmiDebugRuntimeWorkbenchContext context;

    assert(umi_debug_runtime_platform_create(&runtime) == UMI_STATUS_OK);

    umi_toolchain_profile_init(&toolchain);
    (void)memset(&bindings, 0, sizeof(bindings));
    bindings.toolchain = &toolchain;
    assert(umi_developer_workbench_create(
        &bindings, &workbench) == UMI_STATUS_OK);

    assert(umi_debug_runtime_workbench_bridge_create(
        workbench, runtime, &bridge) == UMI_STATUS_OK);
    assert(umi_debug_runtime_workbench_bridge_bind(bridge) ==
           UMI_STATUS_OK);

    (void)memset(&context, 0, sizeof(context));
    (void)strcpy(context.profile_id, "debug.adapter.gdb-dap");
    (void)strcpy(context.session_id, "debug.1");
    (void)strcpy(context.configuration_id, "config.1");
    context.timeout_ms = 100U;

    assert(umi_debug_runtime_workbench_bridge_set_context(
        bridge, &context) == UMI_STATUS_OK);
    assert(umi_developer_workbench_command_enabled(
        workbench, "debug.start", NULL));
    assert(!umi_developer_workbench_command_enabled(
        workbench, "debug.continue", NULL));

    umi_debug_runtime_workbench_bridge_destroy(bridge);
    umi_developer_workbench_destroy(workbench);
    umi_debug_runtime_platform_destroy(runtime);
    return 0;
}
