/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/host_process_catalogue.h
 *
 * PURPOSE:
 *   Maintain bounded isolated host-process state.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_HOST_PROCESS_CATALOGUE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_HOST_PROCESS_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host host process catalogue item data shared with callers
 * of this public contract.
 */
typedef struct UmiPluginExtensionHostHostProcessCatalogueItem { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t state; uint32_t priority; uint64_t evidence; } UmiPluginExtensionHostHostProcessCatalogueItem;
/**
 * Represent the plugin extension host host process catalogue data shared with callers of
 * this public contract.
 */
typedef struct UmiPluginExtensionHostHostProcessCatalogue { UmiPluginExtensionHostHostProcessCatalogueItem items[UMI_PLUGIN_EXTENSION_HOST_MEDIUM_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostHostProcessCatalogue;

/**
 * Initialise plugin extension host host process catalogue from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_host_process_catalogue_init(UmiPluginExtensionHostHostProcessCatalogue *catalogue);
/**
 * Add plugin extension host host process catalogue only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_plugin_extension_host_host_process_catalogue_add(UmiPluginExtensionHostHostProcessCatalogue *catalogue, const char *id, uint32_t state, uint32_t priority, uint64_t evidence);
/**
 * Find plugin extension host host process catalogue while leaving the underlying catalogue
 * or model owned by this module.
 */
const UmiPluginExtensionHostHostProcessCatalogueItem *umi_plugin_extension_host_host_process_catalogue_find(const UmiPluginExtensionHostHostProcessCatalogue *catalogue, const char *id);
/**
 * Remove plugin extension host host process catalogue while keeping the remaining records
 * in a valid and discoverable state.
 */
UmiStatus umi_plugin_extension_host_host_process_catalogue_remove(UmiPluginExtensionHostHostProcessCatalogue *catalogue, const char *id);
/**
 * Return the number of records represented by plugin extension host host process catalogue
 * without changing their state.
 */
size_t umi_plugin_extension_host_host_process_catalogue_count(const UmiPluginExtensionHostHostProcessCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
