/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/crash_catalogue.h
 *
 * PURPOSE:
 *   Maintain bounded recent crash evidence per extension.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_CRASH_CATALOGUE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_CRASH_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostCrashCatalogueItem { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t state; uint32_t priority; uint64_t evidence; } UmiPluginExtensionHostCrashCatalogueItem;
typedef struct UmiPluginExtensionHostCrashCatalogue { UmiPluginExtensionHostCrashCatalogueItem items[UMI_PLUGIN_EXTENSION_HOST_MEDIUM_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostCrashCatalogue;

void umi_plugin_extension_host_crash_catalogue_init(UmiPluginExtensionHostCrashCatalogue *catalogue);
UmiStatus umi_plugin_extension_host_crash_catalogue_add(UmiPluginExtensionHostCrashCatalogue *catalogue, const char *id, uint32_t state, uint32_t priority, uint64_t evidence);
const UmiPluginExtensionHostCrashCatalogueItem *umi_plugin_extension_host_crash_catalogue_find(const UmiPluginExtensionHostCrashCatalogue *catalogue, const char *id);
UmiStatus umi_plugin_extension_host_crash_catalogue_remove(UmiPluginExtensionHostCrashCatalogue *catalogue, const char *id);
size_t umi_plugin_extension_host_crash_catalogue_count(const UmiPluginExtensionHostCrashCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
