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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

void umi_plugin_extension_host_package_identity_init(UmiPluginExtensionHostPackageIdentity *value);
UmiStatus umi_plugin_extension_host_package_identity_configure(UmiPluginExtensionHostPackageIdentity *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
UmiStatus umi_plugin_extension_host_package_identity_validate(const UmiPluginExtensionHostPackageIdentity *value);
uint64_t umi_plugin_extension_host_package_identity_fingerprint(const UmiPluginExtensionHostPackageIdentity *value);

#ifdef __cplusplus
}
#endif

#endif
