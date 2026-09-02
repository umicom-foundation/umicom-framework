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

/**
 * Represent the plugin extension host extension point catalogue item data shared with
 * callers of this public contract.
 */
typedef struct UmiPluginExtensionHostExtensionPointCatalogueItem { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t state; uint32_t priority; uint64_t evidence; } UmiPluginExtensionHostExtensionPointCatalogueItem;
/**
 * Represent the plugin extension host extension point catalogue data shared with callers
 * of this public contract.
 */
typedef struct UmiPluginExtensionHostExtensionPointCatalogue { UmiPluginExtensionHostExtensionPointCatalogueItem items[UMI_PLUGIN_EXTENSION_HOST_MEDIUM_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostExtensionPointCatalogue;

/**
 * Initialise plugin extension host extension point catalogue from caller-provided values
 * so later operations receive a known state.
 */
void umi_plugin_extension_host_extension_point_catalogue_init(UmiPluginExtensionHostExtensionPointCatalogue *catalogue);
/**
 * Add plugin extension host extension point catalogue only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_plugin_extension_host_extension_point_catalogue_add(UmiPluginExtensionHostExtensionPointCatalogue *catalogue, const char *id, uint32_t state, uint32_t priority, uint64_t evidence);
/**
 * Find plugin extension host extension point catalogue while leaving the underlying
 * catalogue or model owned by this module.
 */
const UmiPluginExtensionHostExtensionPointCatalogueItem *umi_plugin_extension_host_extension_point_catalogue_find(const UmiPluginExtensionHostExtensionPointCatalogue *catalogue, const char *id);
/**
 * Remove plugin extension host extension point catalogue while keeping the remaining
 * records in a valid and discoverable state.
 */
UmiStatus umi_plugin_extension_host_extension_point_catalogue_remove(UmiPluginExtensionHostExtensionPointCatalogue *catalogue, const char *id);
/**
 * Return the number of records represented by plugin extension host extension point
 * catalogue without changing their state.
 */
size_t umi_plugin_extension_host_extension_point_catalogue_count(const UmiPluginExtensionHostExtensionPointCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
