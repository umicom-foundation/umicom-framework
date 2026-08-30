/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_compound_configuration.c
 *
 * PURPOSE:
 *   Implement the test debug compound configuration behavior for
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
 * File: tests/test_debug_compound_configuration.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/debug/compound_configuration.h"

static void add_launch(UmiDebugLaunchConfigurationRegistry *registry,
                       const char *id)
{
    UmiDebugLaunchConfigurationSnapshot launch = {0};
    (void)strcpy(launch.id, id);
    (void)strcpy(launch.adapter, "gdb");
    (void)strcpy(launch.program, "/workspace/app");
    assert(umi_debug_launch_configuration_registry_upsert(registry, &launch) ==
           UMI_STATUS_OK);
}

int main(void)
{
    UmiDebugLaunchConfigurationRegistry *configurations = NULL;
    UmiDebugAdapterProfileRegistry *adapters = NULL;
    UmiDebugCompoundConfiguration *compound = NULL;
    UmiDebugAdapterProfile adapter = {0};
    UmiDebugCompoundDescriptor descriptor = {0};
    UmiDebugCompoundEntry first = {0};
    UmiDebugCompoundEntry second = {0};
    UmiDebugCompoundEntry result;
    UmiDebugCompoundSnapshot snapshot;

    (void)strcpy(adapter.id, "gdb");
    (void)strcpy(adapter.executable, "gdb");
    adapter.enabled = 1;
    adapter.supports_launch = 1;
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_DEBUG_COMPOUND_CONFIGURATION_API_VERSION;
    (void)strcpy(descriptor.id, "frontend-and-service");
    (void)strcpy(descriptor.label, "Frontend and service");
    descriptor.launch_mode = UMI_DEBUG_COMPOUND_LAUNCH_SEQUENTIAL;
    descriptor.stop_all = 1;
    first.struct_size = (uint32_t)sizeof(first);
    first.api_version = UMI_DEBUG_COMPOUND_CONFIGURATION_API_VERSION;
    (void)strcpy(first.configuration_id, "service");
    first.order = 20;
    first.enabled = 1;
    first.required = 1;
    second = first;
    (void)strcpy(second.configuration_id, "frontend");
    second.order = 10;
    second.required = 0;

    assert(umi_debug_launch_configuration_registry_create(&configurations) ==
           UMI_STATUS_OK);
    assert(umi_debug_adapter_profile_registry_create(&adapters) ==
           UMI_STATUS_OK);
    assert(umi_debug_compound_configuration_create(&compound) == UMI_STATUS_OK);
    assert(umi_debug_adapter_profile_registry_upsert(adapters, &adapter) ==
           UMI_STATUS_OK);
    add_launch(configurations, "service");
    add_launch(configurations, "frontend");
    assert(umi_debug_compound_configuration_begin(compound, &descriptor) ==
           UMI_STATUS_OK);
    assert(umi_debug_compound_configuration_add(compound, &first) ==
           UMI_STATUS_OK);
    assert(umi_debug_compound_configuration_add(compound, &second) ==
           UMI_STATUS_OK);
    assert(umi_debug_compound_configuration_finalize(
               compound, configurations, adapters) == UMI_STATUS_OK);
    assert(umi_debug_compound_configuration_at(compound, 0U, &result) ==
           UMI_STATUS_OK);
    assert(strcmp(result.configuration_id, "frontend") == 0);
    assert(result.launchable);
    assert(umi_debug_compound_configuration_snapshot(compound, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.state == UMI_DEBUG_COMPOUND_READY);
    assert(snapshot.entry_count == 2U);
    assert(snapshot.launchable_count == 2U);
    assert(snapshot.required_count == 1U);
    assert(snapshot.stop_all);
    umi_debug_compound_configuration_destroy(compound);
    umi_debug_adapter_profile_registry_destroy(adapters);
    umi_debug_launch_configuration_registry_destroy(configurations);
    return 0;
}
