/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/package_catalogue.h
 *
 * PURPOSE:
 *   Maintain a bounded catalogue of installed extension package identities.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_PACKAGE_CATALOGUE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_PACKAGE_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostPackageCatalogueItem { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t state; uint32_t priority; uint64_t evidence; } UmiPluginExtensionHostPackageCatalogueItem;
typedef struct UmiPluginExtensionHostPackageCatalogue { UmiPluginExtensionHostPackageCatalogueItem items[UMI_PLUGIN_EXTENSION_HOST_MEDIUM_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostPackageCatalogue;

void umi_plugin_extension_host_package_catalogue_init(UmiPluginExtensionHostPackageCatalogue *catalogue);
UmiStatus umi_plugin_extension_host_package_catalogue_add(UmiPluginExtensionHostPackageCatalogue *catalogue, const char *id, uint32_t state, uint32_t priority, uint64_t evidence);
const UmiPluginExtensionHostPackageCatalogueItem *umi_plugin_extension_host_package_catalogue_find(const UmiPluginExtensionHostPackageCatalogue *catalogue, const char *id);
UmiStatus umi_plugin_extension_host_package_catalogue_remove(UmiPluginExtensionHostPackageCatalogue *catalogue, const char *id);
size_t umi_plugin_extension_host_package_catalogue_count(const UmiPluginExtensionHostPackageCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
