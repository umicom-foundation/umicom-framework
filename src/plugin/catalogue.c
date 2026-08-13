/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/catalogue.c
 *
 * PURPOSE:
 *   Implement a bounded local catalogue and deterministic marketplace query.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/plugin/catalogue.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UmiPluginCatalogue {
    UmiPluginCatalogueEntry items[UMI_PLUGIN_CATALOGUE_MAX];
    size_t count;
};

static int text_contains_folded(const char *text, const char *query)
{
    size_t text_index;
    size_t query_length;
    if (query == NULL || query[0] == '\0') return 1;
    query_length = strlen(query);
    for (text_index = 0U; text[text_index] != '\0'; ++text_index) {
        size_t offset = 0U;
        while (offset < query_length && text[text_index + offset] != '\0' &&
               tolower((unsigned char)text[text_index + offset]) ==
               tolower((unsigned char)query[offset])) ++offset;
        if (offset == query_length) return 1;
    }
    return 0;
}

static int version_newer(UmiVersion candidate, UmiVersion installed)
{
    if (candidate.major != installed.major) return candidate.major > installed.major;
    if (candidate.minor != installed.minor) return candidate.minor > installed.minor;
    return candidate.patch > installed.patch;
}

UmiStatus umi_plugin_catalogue_create(UmiPluginCatalogue **out_catalogue)
{
    if (out_catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = (UmiPluginCatalogue *)calloc(1U, sizeof(**out_catalogue));
    return *out_catalogue != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

void umi_plugin_catalogue_destroy(UmiPluginCatalogue *catalogue)
{
    free(catalogue);
}

UmiStatus umi_plugin_catalogue_add(UmiPluginCatalogue *catalogue,
                                   const UmiPluginCatalogueEntry *entry)
{
    size_t index;
    if (catalogue == NULL || entry == NULL || entry->plugin_id[0] == '\0' ||
        entry->display_name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].plugin_id, entry->plugin_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (catalogue->count >= UMI_PLUGIN_CATALOGUE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count++] = *entry;
    return UMI_STATUS_OK;
}

size_t umi_plugin_catalogue_count(const UmiPluginCatalogue *catalogue)
{
    return catalogue != NULL ? catalogue->count : 0U;
}

UmiStatus umi_plugin_catalogue_at(const UmiPluginCatalogue *catalogue,
                                  size_t index,
                                  UmiPluginCatalogueEntry *out_entry)
{
    if (catalogue == NULL || out_entry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = catalogue->items[index];
    return UMI_STATUS_OK;
}

size_t umi_plugin_catalogue_query(const UmiPluginCatalogue *catalogue,
                                  const UmiPluginCatalogueQuery *query,
                                  UmiPluginCatalogueEntry *out_entries,
                                  size_t capacity)
{
    size_t index;
    size_t count = 0U;
    if (catalogue == NULL || query == NULL || (capacity > 0U && out_entries == NULL)) {
        return 0U;
    }
    for (index = 0U; index < catalogue->count && count < capacity; ++index) {
        const UmiPluginCatalogueEntry *entry = &catalogue->items[index];
        int text_match = text_contains_folded(entry->display_name, query->text) ||
                         text_contains_folded(entry->plugin_id, query->text) ||
                         text_contains_folded(entry->publisher, query->text) ||
                         text_contains_folded(entry->description, query->text);
        if (!text_match || (query->verified_only && !entry->verified) ||
            (!query->include_deprecated && entry->deprecated) ||
            (query->framework_abi > 0U && entry->required_abi > query->framework_abi)) {
            continue;
        }
        out_entries[count++] = *entry;
    }
    return count;
}

int umi_plugin_catalogue_update_available(const UmiPluginCatalogueEntry *entry,
                                          UmiVersion installed_version)
{
    return entry != NULL && version_newer(entry->version, installed_version);
}
