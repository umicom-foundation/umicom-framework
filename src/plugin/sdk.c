/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/sdk.c
 *
 * PURPOSE:
 *   Validate stable Extension SDK descriptors and expose versioned host
 *   services through a bounded provider-neutral registry.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/plugin/sdk.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiPluginSdkServiceRegistry {
    UmiPluginSdkService items[UMI_PLUGIN_SDK_SERVICE_MAX];
    size_t count;
};

UmiStatus umi_plugin_sdk_descriptor_validate(
    const UmiPluginSdkDescriptor *descriptor,
    uint32_t host_sdk_abi,
    uint32_t framework_abi,
    char *out_reason,
    size_t reason_capacity)
{
    if (descriptor == NULL || out_reason == NULL || reason_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (descriptor->struct_size < sizeof(*descriptor)) {
        (void)snprintf(out_reason, reason_capacity,
                       "SDK descriptor structure is too small");
        return UMI_STATUS_INVALID_STATE;
    }
    if (descriptor->sdk_abi_version != host_sdk_abi) {
        (void)snprintf(out_reason, reason_capacity,
                       "SDK ABI %u does not match host ABI %u",
                       descriptor->sdk_abi_version, host_sdk_abi);
        return UMI_STATUS_INVALID_STATE;
    }
    if (descriptor->minimum_framework_abi > framework_abi) {
        (void)snprintf(out_reason, reason_capacity,
                       "extension requires Framework ABI %u",
                       descriptor->minimum_framework_abi);
        return UMI_STATUS_INVALID_STATE;
    }
    if (descriptor->plugin_id[0] == '\0' || descriptor->entry_symbol[0] == '\0') {
        (void)snprintf(out_reason, reason_capacity,
                       "extension identity and entry symbol are required");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)snprintf(out_reason, reason_capacity, "SDK descriptor is compatible");
    return UMI_STATUS_OK;
}

UmiStatus umi_plugin_sdk_service_registry_create(
    UmiPluginSdkServiceRegistry **out_registry)
{
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = (UmiPluginSdkServiceRegistry *)calloc(1U, sizeof(**out_registry));
    return *out_registry != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

void umi_plugin_sdk_service_registry_destroy(UmiPluginSdkServiceRegistry *registry)
{
    free(registry);
}

UmiStatus umi_plugin_sdk_service_registry_add(
    UmiPluginSdkServiceRegistry *registry,
    const UmiPluginSdkService *service)
{
    size_t index;
    if (registry == NULL || service == NULL || service->service_id[0] == '\0' ||
        service->version == 0U || service->service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].service_id, service->service_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (registry->count >= UMI_PLUGIN_SDK_SERVICE_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->items[registry->count++] = *service;
    return UMI_STATUS_OK;
}

const void *umi_plugin_sdk_service_registry_resolve(
    const UmiPluginSdkServiceRegistry *registry,
    const char *service_id,
    uint32_t minimum_version)
{
    size_t index;
    if (registry == NULL || service_id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index) {
        const UmiPluginSdkService *service = &registry->items[index];
        if (strcmp(service->service_id, service_id) == 0 &&
            service->version >= minimum_version) return service->service;
    }
    return NULL;
}

size_t umi_plugin_sdk_service_registry_count(
    const UmiPluginSdkServiceRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
