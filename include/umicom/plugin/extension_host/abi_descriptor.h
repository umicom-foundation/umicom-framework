/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/abi_descriptor.h
 *
 * PURPOSE:
 *   Describe the stable C ABI contract expected by one extension and its host.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_ABI_DESCRIPTOR_H
#define UMICOM_PLUGIN_EXTENSION_HOST_ABI_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host abi descriptor data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostAbiDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY];
    char subject[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY];
    uint32_t version;
    uint32_t risk;
    uint64_t flags;
    uint64_t revision;
} UmiPluginExtensionHostAbiDescriptor;

/**
 * Initialise plugin extension host abi descriptor from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_abi_descriptor_init(UmiPluginExtensionHostAbiDescriptor *value);
/**
 * Provide the plugin extension host abi descriptor configure operation used by this module
 * and its client applications.
 */
UmiStatus umi_plugin_extension_host_abi_descriptor_configure(UmiPluginExtensionHostAbiDescriptor *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
/**
 * Check that plugin extension host abi descriptor satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_plugin_extension_host_abi_descriptor_validate(const UmiPluginExtensionHostAbiDescriptor *value);
/**
 * Provide the plugin extension host abi descriptor fingerprint operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_abi_descriptor_fingerprint(const UmiPluginExtensionHostAbiDescriptor *value);

#ifdef __cplusplus
}
#endif

#endif
