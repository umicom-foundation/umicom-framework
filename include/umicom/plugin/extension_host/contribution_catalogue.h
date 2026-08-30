/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/contribution_catalogue.h
 *
 * PURPOSE:
 *   Catalogue extension contributions independently of frontend toolkits.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_CONTRIBUTION_CATALOGUE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_CONTRIBUTION_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostContributionCatalogueItem { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t state; uint32_t priority; uint64_t evidence; } UmiPluginExtensionHostContributionCatalogueItem;
typedef struct UmiPluginExtensionHostContributionCatalogue { UmiPluginExtensionHostContributionCatalogueItem items[UMI_PLUGIN_EXTENSION_HOST_MEDIUM_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostContributionCatalogue;

void umi_plugin_extension_host_contribution_catalogue_init(UmiPluginExtensionHostContributionCatalogue *catalogue);
UmiStatus umi_plugin_extension_host_contribution_catalogue_add(UmiPluginExtensionHostContributionCatalogue *catalogue, const char *id, uint32_t state, uint32_t priority, uint64_t evidence);
const UmiPluginExtensionHostContributionCatalogueItem *umi_plugin_extension_host_contribution_catalogue_find(const UmiPluginExtensionHostContributionCatalogue *catalogue, const char *id);
UmiStatus umi_plugin_extension_host_contribution_catalogue_remove(UmiPluginExtensionHostContributionCatalogue *catalogue, const char *id);
size_t umi_plugin_extension_host_contribution_catalogue_count(const UmiPluginExtensionHostContributionCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
