/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/views/command_search.c
 *
 * PURPOSE:
 *   Project command and menu search results for a professional command palette.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/views/command_search.h"

#include <stdio.h>

UmiStatus umi_studio_command_search_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    const char *query,
    UmiUiViewModel **out_view)
{
    UmiStudioRuntimeCommandSearchResults results;
    UmiStudioRuntimeBindings *bindings;
    size_t index;
    UmiStatus status;

    if (platform == NULL || query == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bindings = umi_studio_runtime_platform_bindings(platform);
    if (bindings == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_studio_command_search(bindings, query, &results);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_create_base(
        view_id,
        "studio-command-search",
        "Command Search",
        "Search commands and menu contributions from one authoritative registry.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_set_string(
        *out_view, "studio.command-search.query", query);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view, "studio.command-search.count", (int64_t)results.count);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_boolean(
            *out_view, "studio.command-search.truncated", results.truncated);

    for (index = 0U;
         status == UMI_STATUS_OK && index < results.count;
         ++index) {
        const UmiStudioRuntimeCommandSearchResult *item =
            &results.items[index];
        char key[96];
        char row[512];

        (void)snprintf(
            key, sizeof(key), "studio.command-search.row.%zu", index);
        (void)snprintf(
            row,
            sizeof(row),
            "%.180s | %.160s | %.80s | score=%u%s",
            item->title,
            item->command_id,
            item->category,
            item->score,
            item->enabled ? "" : " | disabled");

        status = umi_studio_view_set_string(*out_view, key, row);
    }

    return status;
}
