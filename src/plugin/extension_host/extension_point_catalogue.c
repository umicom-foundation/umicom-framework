/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/extension_point_catalogue.c
 *
 * PURPOSE:
 *   Catalogue Framework extension points available to extensions.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/extension_point_catalogue.h"
#include <string.h>

static void umi_plugin_extension_host_extension_point_catalogue_copy(char *destination, size_t capacity, const char *source)
{
    size_t i = 0U;
    if (destination == NULL || capacity == 0U) return;
    if (source != NULL) {
        while (i + 1U < capacity && source[i] != '\0') { destination[i] = source[i]; ++i; }
    }
    destination[i] = '\0';
}

void umi_plugin_extension_host_extension_point_catalogue_init(UmiPluginExtensionHostExtensionPointCatalogue *catalogue) { if (catalogue != NULL) memset(catalogue, 0, sizeof(*catalogue)); }
const UmiPluginExtensionHostExtensionPointCatalogueItem *umi_plugin_extension_host_extension_point_catalogue_find(const UmiPluginExtensionHostExtensionPointCatalogue *catalogue, const char *id)
{
    size_t i; if (catalogue == NULL || id == NULL) return NULL;
    for (i=0U;i<catalogue->count;++i) if (strcmp(catalogue->items[i].id,id)==0) return &catalogue->items[i];
    return NULL;
}
UmiStatus umi_plugin_extension_host_extension_point_catalogue_add(UmiPluginExtensionHostExtensionPointCatalogue *catalogue, const char *id, uint32_t state, uint32_t priority, uint64_t evidence)
{
    size_t i; if (catalogue == NULL || !umi_plugin_extension_host_text_valid(id,UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<catalogue->count;++i) if (strcmp(catalogue->items[i].id,id)==0) return UMI_STATUS_ALREADY_EXISTS;
    if (catalogue->count >= UMI_PLUGIN_EXTENSION_HOST_MEDIUM_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    i=catalogue->count++; umi_plugin_extension_host_extension_point_catalogue_copy(catalogue->items[i].id,sizeof(catalogue->items[i].id),id); catalogue->items[i].state=state; catalogue->items[i].priority=priority; catalogue->items[i].evidence=evidence; ++catalogue->revision; return UMI_STATUS_OK;
}
UmiStatus umi_plugin_extension_host_extension_point_catalogue_remove(UmiPluginExtensionHostExtensionPointCatalogue *catalogue, const char *id)
{
    size_t i,j; if (catalogue==NULL||id==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<catalogue->count;++i) if(strcmp(catalogue->items[i].id,id)==0) { for(j=i+1U;j<catalogue->count;++j) catalogue->items[j-1U]=catalogue->items[j]; --catalogue->count; ++catalogue->revision; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
size_t umi_plugin_extension_host_extension_point_catalogue_count(const UmiPluginExtensionHostExtensionPointCatalogue *catalogue) { return catalogue==NULL?0U:catalogue->count; }
