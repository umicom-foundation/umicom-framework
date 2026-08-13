/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/catalogue.h
 *
 * PURPOSE:
 *   Maintain a provider-neutral extension catalogue with query, compatibility
 *   and update information suitable for local or remote marketplace adapters.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_CATALOGUE_H
#define UMICOM_PLUGIN_CATALOGUE_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/plugin/manifest.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLUGIN_CATALOGUE_MAX 1024U
#define UMI_PLUGIN_DESCRIPTION_CAPACITY 512U

typedef struct UmiPluginCatalogueEntry {
    char plugin_id[UMI_PLUGIN_ID_CAPACITY];
    char display_name[UMI_PLUGIN_NAME_CAPACITY];
    char publisher[UMI_PLUGIN_NAME_CAPACITY];
    char description[UMI_PLUGIN_DESCRIPTION_CAPACITY];
    UmiVersion version;
    uint32_t required_abi;
    uint64_t download_size;
    unsigned int rating_milli;
    int verified;
    int deprecated;
} UmiPluginCatalogueEntry;

typedef struct UmiPluginCatalogue UmiPluginCatalogue;

typedef struct UmiPluginCatalogueQuery {
    const char *text;
    uint32_t framework_abi;
    int verified_only;
    int include_deprecated;
} UmiPluginCatalogueQuery;

UmiStatus umi_plugin_catalogue_create(UmiPluginCatalogue **out_catalogue);
void umi_plugin_catalogue_destroy(UmiPluginCatalogue *catalogue);
UmiStatus umi_plugin_catalogue_add(UmiPluginCatalogue *catalogue,
                                   const UmiPluginCatalogueEntry *entry);
size_t umi_plugin_catalogue_count(const UmiPluginCatalogue *catalogue);
UmiStatus umi_plugin_catalogue_at(const UmiPluginCatalogue *catalogue,
                                  size_t index,
                                  UmiPluginCatalogueEntry *out_entry);
size_t umi_plugin_catalogue_query(const UmiPluginCatalogue *catalogue,
                                  const UmiPluginCatalogueQuery *query,
                                  UmiPluginCatalogueEntry *out_entries,
                                  size_t capacity);
int umi_plugin_catalogue_update_available(const UmiPluginCatalogueEntry *entry,
                                          UmiVersion installed_version);

#ifdef __cplusplus
}
#endif

#endif
