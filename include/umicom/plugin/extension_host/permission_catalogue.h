/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/permission_catalogue.h
 *
 * PURPOSE:
 *   Maintain bounded effective extension permission decisions.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_PERMISSION_CATALOGUE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_PERMISSION_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostPermissionCatalogueItem { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t state; uint32_t priority; uint64_t evidence; } UmiPluginExtensionHostPermissionCatalogueItem;
typedef struct UmiPluginExtensionHostPermissionCatalogue { UmiPluginExtensionHostPermissionCatalogueItem items[UMI_PLUGIN_EXTENSION_HOST_MEDIUM_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostPermissionCatalogue;

void umi_plugin_extension_host_permission_catalogue_init(UmiPluginExtensionHostPermissionCatalogue *catalogue);
UmiStatus umi_plugin_extension_host_permission_catalogue_add(UmiPluginExtensionHostPermissionCatalogue *catalogue, const char *id, uint32_t state, uint32_t priority, uint64_t evidence);
const UmiPluginExtensionHostPermissionCatalogueItem *umi_plugin_extension_host_permission_catalogue_find(const UmiPluginExtensionHostPermissionCatalogue *catalogue, const char *id);
UmiStatus umi_plugin_extension_host_permission_catalogue_remove(UmiPluginExtensionHostPermissionCatalogue *catalogue, const char *id);
size_t umi_plugin_extension_host_permission_catalogue_count(const UmiPluginExtensionHostPermissionCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
