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

/**
 * Represent the plugin sdk host api data shared with callers of this public contract.
 */
typedef struct UmiPluginSdkHostApi {
    uint32_t struct_size;
    uint32_t abi_version;
    void *host_context;
    UmiPluginSdkLogFn log;
    UmiPluginSdkCommandFn execute_command;
    UmiPluginSdkResolveFn resolve_service;
} UmiPluginSdkHostApi;

/**
 * Represent the plugin sdk descriptor data shared with callers of this public contract.
 */
typedef struct UmiPluginSdkDescriptor {
    uint32_t struct_size;
    uint32_t sdk_abi_version;
    uint32_t minimum_framework_abi;
    UmiVersion extension_version;
    char plugin_id[UMI_PLUGIN_ID_CAPACITY];
    char entry_symbol[UMI_PLUGIN_ITEM_CAPACITY];
    uint64_t capability_flags;
} UmiPluginSdkDescriptor;

/**
 * Represent the plugin sdk service data shared with callers of this public contract.
 */
typedef struct UmiPluginSdkService {
    char service_id[UMI_PLUGIN_SDK_SERVICE_ID_CAPACITY];
    uint32_t version;
    const void *service;
} UmiPluginSdkService;

/**
 * Represent the plugin sdk service registry data shared with callers of this public
 * contract.
 */
typedef struct UmiPluginSdkServiceRegistry UmiPluginSdkServiceRegistry;

/**
 * Check that plugin sdk descriptor satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_plugin_sdk_descriptor_validate(
    const UmiPluginSdkDescriptor *descriptor,
    uint32_t host_sdk_abi,
    uint32_t framework_abi,
    char *out_reason,
    size_t reason_capacity);
/**
 * Initialise plugin sdk service registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_plugin_sdk_service_registry_create(
    UmiPluginSdkServiceRegistry **out_registry);
/**
 * Release or reset state held by plugin sdk service registry so the same storage can be
 * reused safely.
 */
void umi_plugin_sdk_service_registry_destroy(UmiPluginSdkServiceRegistry *registry);
/**
 * Add plugin sdk service registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_plugin_sdk_service_registry_add(
    UmiPluginSdkServiceRegistry *registry,
    const UmiPluginSdkService *service);
/**
 * Provide the plugin sdk service registry resolve operation used by this module and its
 * client applications.
 */
const void *umi_plugin_sdk_service_registry_resolve(
    const UmiPluginSdkServiceRegistry *registry,
    const char *service_id,
    uint32_t minimum_version);
/**
 * Return the number of records represented by plugin sdk service registry without changing
 * their state.
 */
size_t umi_plugin_sdk_service_registry_count(
    const UmiPluginSdkServiceRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
