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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the plugin extension host package catalogue item data shared with callers of
 * this public contract.
 */
typedef struct UmiPluginExtensionHostPackageCatalogueItem { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t state; uint32_t priority; uint64_t evidence; } UmiPluginExtensionHostPackageCatalogueItem;
/**
 * Represent the plugin extension host package catalogue data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostPackageCatalogue { UmiPluginExtensionHostPackageCatalogueItem items[UMI_PLUGIN_EXTENSION_HOST_MEDIUM_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostPackageCatalogue;

/**
 * Initialise plugin extension host package catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_package_catalogue_init(UmiPluginExtensionHostPackageCatalogue *catalogue);
/**
 * Add plugin extension host package catalogue only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_plugin_extension_host_package_catalogue_add(UmiPluginExtensionHostPackageCatalogue *catalogue, const char *id, uint32_t state, uint32_t priority, uint64_t evidence);
/**
 * Find plugin extension host package catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiPluginExtensionHostPackageCatalogueItem *umi_plugin_extension_host_package_catalogue_find(const UmiPluginExtensionHostPackageCatalogue *catalogue, const char *id);
/**
 * Remove plugin extension host package catalogue while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_plugin_extension_host_package_catalogue_remove(UmiPluginExtensionHostPackageCatalogue *catalogue, const char *id);
/**
 * Return the number of records represented by plugin extension host package catalogue
 * without changing their state.
 */
size_t umi_plugin_extension_host_package_catalogue_count(const UmiPluginExtensionHostPackageCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
