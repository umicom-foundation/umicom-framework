/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_plugin_sdk.c
 *
 * PURPOSE:
 *   Verify stable SDK descriptor validation and versioned service resolution.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "plugin_fixture.h"

int main(void)
{
    static const int example_service = 42;
    UmiPluginSdkDescriptor descriptor;
    UmiPluginSdkServiceRegistry *registry = NULL;
    UmiPluginSdkService service;
    char reason[256];
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.sdk_abi_version = UMI_PLUGIN_SDK_ABI_VERSION;
    descriptor.minimum_framework_abi = UMICOM_FRAMEWORK_ABI_VERSION;
    (void)strcpy(descriptor.plugin_id, "org.example");
    (void)strcpy(descriptor.entry_symbol, "umicom_extension_query");
    assert(umi_plugin_sdk_descriptor_validate(&descriptor,
        UMI_PLUGIN_SDK_ABI_VERSION, UMICOM_FRAMEWORK_ABI_VERSION,
        reason, sizeof(reason)) == UMI_STATUS_OK);
    assert(umi_plugin_sdk_service_registry_create(&registry) == UMI_STATUS_OK);
    (void)memset(&service, 0, sizeof(service));
    (void)strcpy(service.service_id, "umicom.commands");
    service.version = 2U;
    service.service = &example_service;
    assert(umi_plugin_sdk_service_registry_add(registry, &service) == UMI_STATUS_OK);
    assert(umi_plugin_sdk_service_registry_resolve(registry,
        "umicom.commands", 2U) == &example_service);
    assert(umi_plugin_sdk_service_registry_resolve(registry,
        "umicom.commands", 3U) == NULL);
    umi_plugin_sdk_service_registry_destroy(registry);
    return 0;
}
