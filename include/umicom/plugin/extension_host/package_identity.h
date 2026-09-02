/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/package_identity.h
 *
 * PURPOSE:
 *   Describe one extension package identity, version and publisher tuple.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_PACKAGE_IDENTITY_H
#define UMICOM_PLUGIN_EXTENSION_HOST_PACKAGE_IDENTITY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host package identity data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostPackageIdentity {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY];
    char subject[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY];
    uint32_t version;
    uint32_t risk;
    uint64_t flags;
    uint64_t revision;
} UmiPluginExtensionHostPackageIdentity;

/**
 * Initialise plugin extension host package identity from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_package_identity_init(UmiPluginExtensionHostPackageIdentity *value);
/**
 * Provide the plugin extension host package identity configure operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_package_identity_configure(UmiPluginExtensionHostPackageIdentity *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
/**
 * Check that plugin extension host package identity satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_plugin_extension_host_package_identity_validate(const UmiPluginExtensionHostPackageIdentity *value);
/**
 * Provide the plugin extension host package identity fingerprint operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_package_identity_fingerprint(const UmiPluginExtensionHostPackageIdentity *value);

#ifdef __cplusplus
}
#endif

#endif
