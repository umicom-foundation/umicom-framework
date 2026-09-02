/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_recent_items_query.c
 *
 * PURPOSE:
 *   Verify that recent work is filtered and ranked consistently for welcome
 *   pages, launchers and resource pickers in every Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/platform/recent_items.h"

/* Create a complete snapshot so the test follows the same bounded public
 * contract used by application services. */
static UmiRecentItemSnapshot make_item(const char *id,
                                       const char *label,
                                       const char *kind,
                                       uint64_t opened_at,
                                       uint64_t open_count,
                                       int pinned)
{
    UmiRecentItemSnapshot item = {0};

    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = 1U;
    (void)snprintf(item.id, sizeof(item.id), "%s", id);
    (void)snprintf(item.uri, sizeof(item.uri), "file:///work/%s", id);
    (void)snprintf(item.label, sizeof(item.label), "%s", label);
    (void)snprintf(item.kind, sizeof(item.kind), "%s", kind);
    item.last_opened = opened_at;
    item.open_count = open_count;
    item.pinned = pinned;
    return item;
}

int main(void)
{
    UmiRecentItemRegistry *registry = NULL;
    UmiRecentItemSnapshot results[4] = {{0}};
    UmiRecentItemSnapshot item;
    UmiRecentItemQuery query = {0};
    char generated_id[128] = {0};
    size_t count = 0U;

    assert(umi_platform_recent_items_registry_create(&registry) ==
           UMI_STATUS_OK);

    item = make_item("bank", "Bank workspace", "workspace", 500U, 2U, 0);
    assert(umi_platform_recent_items_registry_upsert(registry, &item) ==
           UMI_STATUS_OK);
    item = make_item("studio", "Studio project", "project", 400U, 8U, 1);
    assert(umi_platform_recent_items_registry_upsert(registry, &item) ==
           UMI_STATUS_OK);
    item = make_item("trader", "Trader workspace", "workspace", 600U, 1U, 0);
    assert(umi_platform_recent_items_registry_upsert(registry, &item) ==
           UMI_STATUS_OK);

    /* Pinned work stays at the top even when another item was opened later. */
    assert(umi_platform_recent_items_registry_query(
               registry, NULL, results, 4U, &count) == UMI_STATUS_OK);
    assert(count == 3U);
    assert(strcmp(results[0].id, "studio") == 0);
    assert(strcmp(results[1].id, "trader") == 0);
    assert(strcmp(results[2].id, "bank") == 0);

    /* Search applies to labels, identifiers and URIs without case surprises. */
    query.text = "TRADER";
    query.kind = "workspace";
    query.limit = 1U;
    assert(umi_platform_recent_items_registry_query(
               registry, &query, results, 4U, &count) == UMI_STATUS_OK);
    assert(count == 1U);
    assert(strcmp(results[0].id, "trader") == 0);

    query.text = NULL;
    query.kind = NULL;
    query.pinned_only = 1;
    query.limit = 0U;
    assert(umi_platform_recent_items_registry_query(
               registry, &query, results, 4U, &count) == UMI_STATUS_OK);
    assert(count == 1U);
    assert(results[0].pinned != 0);

    assert(umi_platform_recent_item_id_from_uri(
               "workspace", "file:///work/trader",
               generated_id, sizeof(generated_id)) == UMI_STATUS_OK);
    assert(generated_id[0] != '\0');

    /* Trimming keeps the pinned project and the newest ordinary workspace. */
    assert(umi_platform_recent_items_registry_trim(registry, 2U) ==
           UMI_STATUS_OK);
    assert(umi_platform_recent_items_registry_count(registry) == 2U);
    assert(umi_platform_recent_items_registry_find(
               registry, "bank", &item) == UMI_STATUS_NOT_FOUND);

    umi_platform_recent_items_registry_destroy(registry);
    return 0;
}
