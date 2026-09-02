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

/**
 * Represent the plugin extension host permission catalogue item data shared with callers
 * of this public contract.
 */
typedef struct UmiPluginExtensionHostPermissionCatalogueItem { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t state; uint32_t priority; uint64_t evidence; } UmiPluginExtensionHostPermissionCatalogueItem;
/**
 * Represent the plugin extension host permission catalogue data shared with callers of
 * this public contract.
 */
typedef struct UmiPluginExtensionHostPermissionCatalogue { UmiPluginExtensionHostPermissionCatalogueItem items[UMI_PLUGIN_EXTENSION_HOST_MEDIUM_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostPermissionCatalogue;

/**
 * Initialise plugin extension host permission catalogue from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_permission_catalogue_init(UmiPluginExtensionHostPermissionCatalogue *catalogue);
/**
 * Add plugin extension host permission catalogue only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_plugin_extension_host_permission_catalogue_add(UmiPluginExtensionHostPermissionCatalogue *catalogue, const char *id, uint32_t state, uint32_t priority, uint64_t evidence);
/**
 * Find plugin extension host permission catalogue while leaving the underlying catalogue
 * or model owned by this module.
 */
const UmiPluginExtensionHostPermissionCatalogueItem *umi_plugin_extension_host_permission_catalogue_find(const UmiPluginExtensionHostPermissionCatalogue *catalogue, const char *id);
/**
 * Remove plugin extension host permission catalogue while keeping the remaining records in
 * a valid and discoverable state.
 */
UmiStatus umi_plugin_extension_host_permission_catalogue_remove(UmiPluginExtensionHostPermissionCatalogue *catalogue, const char *id);
/**
 * Return the number of records represented by plugin extension host permission catalogue
 * without changing their state.
 */
size_t umi_plugin_extension_host_permission_catalogue_count(const UmiPluginExtensionHostPermissionCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
