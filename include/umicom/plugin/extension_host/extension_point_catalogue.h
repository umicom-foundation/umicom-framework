/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/extension_point_catalogue.h
 *
 * PURPOSE:
 *   Catalogue Framework extension points available to extensions.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_EXTENSION_POINT_CATALOGUE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_EXTENSION_POINT_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostExtensionPointCatalogueItem { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t state; uint32_t priority; uint64_t evidence; } UmiPluginExtensionHostExtensionPointCatalogueItem;
typedef struct UmiPluginExtensionHostExtensionPointCatalogue { UmiPluginExtensionHostExtensionPointCatalogueItem items[UMI_PLUGIN_EXTENSION_HOST_MEDIUM_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostExtensionPointCatalogue;

void umi_plugin_extension_host_extension_point_catalogue_init(UmiPluginExtensionHostExtensionPointCatalogue *catalogue);
UmiStatus umi_plugin_extension_host_extension_point_catalogue_add(UmiPluginExtensionHostExtensionPointCatalogue *catalogue, const char *id, uint32_t state, uint32_t priority, uint64_t evidence);
const UmiPluginExtensionHostExtensionPointCatalogueItem *umi_plugin_extension_host_extension_point_catalogue_find(const UmiPluginExtensionHostExtensionPointCatalogue *catalogue, const char *id);
UmiStatus umi_plugin_extension_host_extension_point_catalogue_remove(UmiPluginExtensionHostExtensionPointCatalogue *catalogue, const char *id);
size_t umi_plugin_extension_host_extension_point_catalogue_count(const UmiPluginExtensionHostExtensionPointCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
