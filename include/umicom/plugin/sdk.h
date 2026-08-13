/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/sdk.h
 *
 * PURPOSE:
 *   Define the stable C-facing Extension SDK descriptor, host services and
 *   compatibility validation shared by every Umicom product.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_SDK_H
#define UMICOM_PLUGIN_SDK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/plugin/manifest.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLUGIN_SDK_ABI_VERSION 2U
#define UMI_PLUGIN_SDK_SERVICE_MAX 64U
#define UMI_PLUGIN_SDK_SERVICE_ID_CAPACITY 128U

typedef UmiStatus (*UmiPluginSdkLogFn)(void *host_context,
                                      int level,
                                      const char *message);
typedef UmiStatus (*UmiPluginSdkCommandFn)(void *host_context,
                                          const char *command_id,
                                          const char *argument);
typedef const void *(*UmiPluginSdkResolveFn)(void *host_context,
                                            const char *service_id,
                                            uint32_t minimum_version);

typedef struct UmiPluginSdkHostApi {
    uint32_t struct_size;
    uint32_t abi_version;
    void *host_context;
    UmiPluginSdkLogFn log;
    UmiPluginSdkCommandFn execute_command;
    UmiPluginSdkResolveFn resolve_service;
} UmiPluginSdkHostApi;

typedef struct UmiPluginSdkDescriptor {
    uint32_t struct_size;
    uint32_t sdk_abi_version;
    uint32_t minimum_framework_abi;
    UmiVersion extension_version;
    char plugin_id[UMI_PLUGIN_ID_CAPACITY];
    char entry_symbol[UMI_PLUGIN_ITEM_CAPACITY];
    uint64_t capability_flags;
} UmiPluginSdkDescriptor;

typedef struct UmiPluginSdkService {
    char service_id[UMI_PLUGIN_SDK_SERVICE_ID_CAPACITY];
    uint32_t version;
    const void *service;
} UmiPluginSdkService;

typedef struct UmiPluginSdkServiceRegistry UmiPluginSdkServiceRegistry;

UmiStatus umi_plugin_sdk_descriptor_validate(
    const UmiPluginSdkDescriptor *descriptor,
    uint32_t host_sdk_abi,
    uint32_t framework_abi,
    char *out_reason,
    size_t reason_capacity);
UmiStatus umi_plugin_sdk_service_registry_create(
    UmiPluginSdkServiceRegistry **out_registry);
void umi_plugin_sdk_service_registry_destroy(UmiPluginSdkServiceRegistry *registry);
UmiStatus umi_plugin_sdk_service_registry_add(
    UmiPluginSdkServiceRegistry *registry,
    const UmiPluginSdkService *service);
const void *umi_plugin_sdk_service_registry_resolve(
    const UmiPluginSdkServiceRegistry *registry,
    const char *service_id,
    uint32_t minimum_version);
size_t umi_plugin_sdk_service_registry_count(
    const UmiPluginSdkServiceRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
