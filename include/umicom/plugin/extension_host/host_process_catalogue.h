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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiPluginExtensionHostHostProcessCatalogueItem { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t state; uint32_t priority; uint64_t evidence; } UmiPluginExtensionHostHostProcessCatalogueItem;
typedef struct UmiPluginExtensionHostHostProcessCatalogue { UmiPluginExtensionHostHostProcessCatalogueItem items[UMI_PLUGIN_EXTENSION_HOST_MEDIUM_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostHostProcessCatalogue;

void umi_plugin_extension_host_host_process_catalogue_init(UmiPluginExtensionHostHostProcessCatalogue *catalogue);
UmiStatus umi_plugin_extension_host_host_process_catalogue_add(UmiPluginExtensionHostHostProcessCatalogue *catalogue, const char *id, uint32_t state, uint32_t priority, uint64_t evidence);
const UmiPluginExtensionHostHostProcessCatalogueItem *umi_plugin_extension_host_host_process_catalogue_find(const UmiPluginExtensionHostHostProcessCatalogue *catalogue, const char *id);
UmiStatus umi_plugin_extension_host_host_process_catalogue_remove(UmiPluginExtensionHostHostProcessCatalogue *catalogue, const char *id);
size_t umi_plugin_extension_host_host_process_catalogue_count(const UmiPluginExtensionHostHostProcessCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
