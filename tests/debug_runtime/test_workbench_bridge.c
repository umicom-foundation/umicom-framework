/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_workbench_bridge.c
 *
 * PURPOSE:
 *   Verify existing Developer Workbench Debug commands become context-aware DAP
 *   actions without launching an external adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/debug_runtime/workbench_bridge.h"
#include "umicom/security/workspace_trust.h"
#include "umicom/test_runtime/check.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugRuntimePlatform *runtime = NULL;
    UmiToolchainProfile *toolchain =
        (UmiToolchainProfile *)calloc(1U, sizeof(*toolchain));
    UmiDeveloperWorkbenchBindings bindings;
    UmiDeveloperWorkbench *workbench = NULL;
    UmiDeveloperWorkbenchConfiguration *configuration =
        (UmiDeveloperWorkbenchConfiguration *)calloc(1U, sizeof(*configuration));
    UmiWorkspaceTrustStore *trust = NULL;
    UmiDebugRuntimeWorkbenchBridge *bridge = NULL;
    UmiDebugRuntimeWorkbenchContext *context =
        (UmiDebugRuntimeWorkbenchContext *)calloc(1U, sizeof(*context));
    int written;

    UMI_TEST_REQUIRE(toolchain != NULL && context != NULL &&
                     configuration != NULL);
    UMI_TEST_REQUIRE(umi_debug_runtime_platform_create(&runtime) == UMI_STATUS_OK);

    umi_toolchain_profile_init(toolchain);
    umi_developer_workbench_configuration_init(
        configuration, "config.1", "Debug");
    written = snprintf(configuration->source_root,
                       sizeof(configuration->source_root),
                       "%s", "C:/work/example");
    UMI_TEST_REQUIRE(written >= 0 &&
                     (size_t)written < sizeof(configuration->source_root));
    written = snprintf(configuration->build_directory,
                       sizeof(configuration->build_directory),
                       "%s", "build/debug");
    UMI_TEST_REQUIRE(written >= 0 &&
                     (size_t)written < sizeof(configuration->build_directory));
    UMI_TEST_REQUIRE(umi_workspace_trust_store_create(&trust) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_workspace_trust_store_set(
        trust, configuration->source_root, UMI_WORKSPACE_TRUSTED,
        "debug-runtime-test", 1U) == UMI_STATUS_OK);
    (void)memset(&bindings, 0, sizeof(bindings));
    bindings.toolchain = toolchain;
    bindings.trust_store = trust;
    UMI_TEST_REQUIRE(umi_developer_workbench_create(
        &bindings, &workbench) == UMI_STATUS_OK);
    /* Debug start is a trusted executable action. The fixture installs a real
     * active configuration instead of bypassing that production safeguard. */
    UMI_TEST_REQUIRE(umi_developer_workbench_add_configuration(
        workbench, configuration, 1) == UMI_STATUS_OK);

    UMI_TEST_REQUIRE(umi_debug_runtime_workbench_bridge_create(
        workbench, runtime, &bridge) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_debug_runtime_workbench_bridge_bind(bridge) ==
           UMI_STATUS_OK);

    /* Bounded formatting makes the fixture safe if an identifier capacity is
     * reduced later and verifies that no test value was truncated. */
    written = snprintf(context->profile_id, sizeof(context->profile_id),
                       "%s", "debug.adapter.gdb-dap");
    UMI_TEST_REQUIRE(written >= 0 &&
                     (size_t)written < sizeof(context->profile_id));
    written = snprintf(context->session_id, sizeof(context->session_id),
                       "%s", "debug.1");
    UMI_TEST_REQUIRE(written >= 0 &&
                     (size_t)written < sizeof(context->session_id));
    written = snprintf(context->configuration_id,
                       sizeof(context->configuration_id), "%s", "config.1");
    UMI_TEST_REQUIRE(written >= 0 &&
                     (size_t)written < sizeof(context->configuration_id));
    context->timeout_ms = 100U;

    UMI_TEST_REQUIRE(umi_debug_runtime_workbench_bridge_set_context(
        bridge, context) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_developer_workbench_command_enabled(
        workbench, "debug.start", NULL));
    UMI_TEST_REQUIRE(!umi_developer_workbench_command_enabled(
        workbench, "debug.continue", NULL));

    umi_debug_runtime_workbench_bridge_destroy(bridge);
    bridge = NULL;
    /* Destroying the bridge must remove callbacks that borrowed its address. */
    UMI_TEST_REQUIRE(!umi_developer_workbench_command_enabled(
        workbench, "debug.start", NULL));
    umi_developer_workbench_destroy(workbench);
    umi_workspace_trust_store_destroy(trust);
    umi_debug_runtime_platform_destroy(runtime);
    free(configuration);
    free(context);
    free(toolchain);
    return 0;
}
