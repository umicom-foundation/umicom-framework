/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/permission_grant.h
 *
 * PURPOSE:
 *   Describe one persisted permission grant and its decision provenance.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_PERMISSION_GRANT_H
#define UMICOM_PLUGIN_EXTENSION_HOST_PERMISSION_GRANT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostPermissionGrant {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY];
    char subject[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY];
    uint32_t version;
    uint32_t risk;
    uint64_t flags;
    uint64_t revision;
} UmiPluginExtensionHostPermissionGrant;

void umi_plugin_extension_host_permission_grant_init(UmiPluginExtensionHostPermissionGrant *value);
UmiStatus umi_plugin_extension_host_permission_grant_configure(UmiPluginExtensionHostPermissionGrant *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
UmiStatus umi_plugin_extension_host_permission_grant_validate(const UmiPluginExtensionHostPermissionGrant *value);
uint64_t umi_plugin_extension_host_permission_grant_fingerprint(const UmiPluginExtensionHostPermissionGrant *value);

#ifdef __cplusplus
}
#endif

#endif
