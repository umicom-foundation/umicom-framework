/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/recent_project_provider.c
 *
 * PURPOSE:
 *   Search recent project/workspace items without duplicating the platform MRU
 *   registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/recent_project_provider.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/* Provide the search recent operation used by this module and its client applications. */
static UmiStatus search_recent(
    void *user_data,
    const char *query,
    UmiDeveloperWorkbenchSearchResult *out_results,
    size_t capacity,
    size_t *out_count)
{
    UmiRecentItemRegistry *recent = (UmiRecentItemRegistry *)user_data;
    size_t index;
    size_t used = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (recent == NULL || query == NULL ||
        out_results == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_platform_recent_items_registry_count(recent) &&
         used < capacity;
         ++index) {
        UmiRecentItemSnapshot item;
        int32_t score;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_platform_recent_items_registry_at(
                recent, index, &item) != UMI_STATUS_OK) {
            continue;
        }

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(item.kind, "project") != 0 &&
            strcmp(item.kind, "workspace") != 0) {
            continue;
        }

        score = umi_developer_workbench_search_score(
            query, item.label, item.uri);
        /* Apply this branch only when its contract condition is satisfied. */
        if (score < 0) continue;

        (void)memset(&out_results[used], 0, sizeof(out_results[used]));
        out_results[used].kind =
            UMI_DEVELOPER_WORKBENCH_SEARCH_RECENT_PROJECT;
        copy_text(out_results[used].provider_id,
                  sizeof(out_results[used].provider_id),
                  "developer.search.recent-projects");
        copy_text(out_results[used].result_id,
                  sizeof(out_results[used].result_id),
                  item.id);
        copy_text(out_results[used].title,
                  sizeof(out_results[used].title),
                  item.label);
        copy_text(out_results[used].detail,
                  sizeof(out_results[used].detail),
                  item.uri);
        copy_text(out_results[used].target,
                  sizeof(out_results[used].target),
                  item.uri);
        out_results[used].enabled = 1;
        out_results[used].score = score + (item.pinned ? 50 : 0);
        out_results[used].revision = item.revision;
        ++used;
    }

    *out_count = used;
    return used > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

/*
 * Initialise developer workbench recent project provider from caller-provided values so
 * later operations receive a known state.
 */
void umi_developer_workbench_recent_project_provider_init(
    UmiDeveloperWorkbenchSearchProvider *provider,
    UmiRecentItemRegistry *recent_items)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL) return;

    (void)memset(provider, 0, sizeof(*provider));
    copy_text(provider->provider_id,
              sizeof(provider->provider_id),
              "developer.search.recent-projects");
    copy_text(provider->title,
              sizeof(provider->title),
              "Recent Projects");
    provider->kind = UMI_DEVELOPER_WORKBENCH_SEARCH_RECENT_PROJECT;
    provider->priority = 80;
    provider->user_data = recent_items;
    provider->search = search_recent;
}
