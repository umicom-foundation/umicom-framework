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

/*
 * Provide the text contains folded operation used by this module and its client
 * applications.
 */
static int text_contains_folded(const char *text, const char *query)
{
    size_t text_index;
    size_t query_length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || query[0] == '\0') return 1;
    query_length = strlen(query);
    /* Visit each bounded item once so every record receives the same rule. */
    for (text_index = 0U; text[text_index] != '\0'; ++text_index) {
        size_t offset = 0U;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (offset < query_length && text[text_index + offset] != '\0' &&
               tolower((unsigned char)text[text_index + offset]) ==
               tolower((unsigned char)query[offset])) ++offset;
        /* Apply this branch only when its contract condition is satisfied. */
        if (offset == query_length) return 1;
    }
    return 0;
}

/* Provide the version newer operation used by this module and its client applications. */
static int version_newer(UmiVersion candidate, UmiVersion installed)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (candidate.major != installed.major) return candidate.major > installed.major;
    /* Apply this branch only when its contract condition is satisfied. */
    if (candidate.minor != installed.minor) return candidate.minor > installed.minor;
    return candidate.patch > installed.patch;
}

/*
 * Initialise plugin catalogue from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_plugin_catalogue_create(UmiPluginCatalogue **out_catalogue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = (UmiPluginCatalogue *)calloc(1U, sizeof(**out_catalogue));
    return *out_catalogue != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

/*
 * Release or reset state held by plugin catalogue so the same storage can be reused
 * safely.
 */
void umi_plugin_catalogue_destroy(UmiPluginCatalogue *catalogue)
{
    free(catalogue);
}

/* Add plugin catalogue only after its inputs and available capacity have been checked. */
UmiStatus umi_plugin_catalogue_add(UmiPluginCatalogue *catalogue,
                                   const UmiPluginCatalogueEntry *entry)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || entry == NULL || entry->plugin_id[0] == '\0' ||
        entry->display_name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].plugin_id, entry->plugin_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_PLUGIN_CATALOGUE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count++] = *entry;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by plugin catalogue without changing their
 * state.
 */
size_t umi_plugin_catalogue_count(const UmiPluginCatalogue *catalogue)
{
    return catalogue != NULL ? catalogue->count : 0U;
}

/*
 * Find plugin catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_plugin_catalogue_at(const UmiPluginCatalogue *catalogue,
                                  size_t index,
                                  UmiPluginCatalogueEntry *out_entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || out_entry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= catalogue->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = catalogue->items[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the plugin catalogue query operation used by this module and its client
 * applications.
 */
size_t umi_plugin_catalogue_query(const UmiPluginCatalogue *catalogue,
                                  const UmiPluginCatalogueQuery *query,
                                  UmiPluginCatalogueEntry *out_entries,
                                  size_t capacity)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || query == NULL || (capacity > 0U && out_entries == NULL)) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count && count < capacity; ++index) {
        const UmiPluginCatalogueEntry *entry = &catalogue->items[index];
        int text_match = text_contains_folded(entry->display_name, query->text) ||
                         text_contains_folded(entry->plugin_id, query->text) ||
                         text_contains_folded(entry->publisher, query->text) ||
                         text_contains_folded(entry->description, query->text);
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!text_match || (query->verified_only && !entry->verified) ||
            (!query->include_deprecated && entry->deprecated) ||
            (query->framework_abi > 0U && entry->required_abi > query->framework_abi)) {
            continue;
        }
        out_entries[count++] = *entry;
    }
    return count;
}

/*
 * Provide the plugin catalogue update available operation used by this module and its
 * client applications.
 */
int umi_plugin_catalogue_update_available(const UmiPluginCatalogueEntry *entry,
                                          UmiVersion installed_version)
{
    return entry != NULL && version_newer(entry->version, installed_version);
}
