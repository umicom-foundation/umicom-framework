/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_configuration_resolver.c
 *
 * PURPOSE:
 *   Implement the test debug configuration resolver behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_configuration_resolver.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/debug/configuration_resolver.h"

int main(void)
{
    UmiDebugLaunchConfigurationRegistry *configurations = NULL;
    UmiDebugAdapterProfileRegistry *adapters = NULL;
    UmiDebugConfigurationResolver *resolver = NULL;
    UmiDebugLaunchConfigurationSnapshot launch = {0};
    UmiDebugAdapterProfile adapter = {0};
    UmiDebugConfigurationRequest request = {0};
    UmiDebugResolvedConfiguration result;

    (void)strcpy(adapter.id, "native-gdb");
    (void)strcpy(adapter.display_name, "Native GDB");
    (void)strcpy(adapter.executable, "gdb");
    adapter.supports_launch = 1;
    adapter.enabled = 1;
    (void)strcpy(launch.id, "debug-app");
    (void)strcpy(launch.name, "Debug application");
    (void)strcpy(launch.adapter, adapter.id);
    (void)strcpy(launch.program, "${workspaceFolder}/build/app");
    (void)strcpy(launch.arguments, "--input ${file}");
    (void)strcpy(launch.working_directory, "${fileDirname}");
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEBUG_CONFIGURATION_RESOLVER_API_VERSION;
    (void)strcpy(request.configuration_id, launch.id);
    (void)strcpy(request.workspace_folder, "/workspace/demo");
    (void)strcpy(request.active_file, "/workspace/demo/src/main.c");

    assert(umi_debug_launch_configuration_registry_create(&configurations) ==
           UMI_STATUS_OK);
    assert(umi_debug_adapter_profile_registry_create(&adapters) ==
           UMI_STATUS_OK);
    assert(umi_debug_configuration_resolver_create(&resolver) == UMI_STATUS_OK);
    assert(umi_debug_launch_configuration_registry_upsert(configurations,
                                                          &launch) ==
           UMI_STATUS_OK);
    assert(umi_debug_adapter_profile_registry_upsert(adapters, &adapter) ==
           UMI_STATUS_OK);
    assert(umi_debug_configuration_resolver_resolve(
               resolver, configurations, adapters, &request) == UMI_STATUS_OK);
    assert(umi_debug_configuration_resolver_result(resolver, &result) ==
           UMI_STATUS_OK);
    assert(result.ready);
    assert(result.substituted_token_count == 3U);
    assert(strcmp(result.launch.program, "/workspace/demo/build/app") == 0);
    assert(strcmp(result.launch.arguments,
                  "--input /workspace/demo/src/main.c") == 0);
    assert(strcmp(result.launch.working_directory,
                  "/workspace/demo/src") == 0);
    assert(strcmp(result.adapter.id, "native-gdb") == 0);
    umi_debug_configuration_resolver_destroy(resolver);
    umi_debug_adapter_profile_registry_destroy(adapters);
    umi_debug_launch_configuration_registry_destroy(configurations);
    return 0;
}
