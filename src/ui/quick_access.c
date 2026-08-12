/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/quick_access.c
 *
 * PURPOSE:
 *   Implement deterministic fuzzy ranking over the Framework command registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The command registry remains the source of truth. Quick Access creates temporary ranked snapshots and never owns command handlers.
 */

#include "umicom/ui/quick_access.h"

#include <stdio.h>
#include <string.h>

#include "workbench_service_internal.h"

static void insert_sorted(UmiUiQuickAccessResults *results,
                          const UmiUiQuickAccessItem *item)
{
    size_t position = results->count;
    size_t index;
    if (position < UMI_UI_QUICK_ACCESS_MAX_RESULTS) {
        ++results->count;
    } else if (results->items[position - 1U].score >= item->score) {
        return;
    } else {
        position = UMI_UI_QUICK_ACCESS_MAX_RESULTS - 1U;
    }

    if (position >= results->count) position = results->count - 1U;
    while (position > 0U &&
           results->items[position - 1U].score < item->score) {
        if (position < UMI_UI_QUICK_ACCESS_MAX_RESULTS) {
            results->items[position] = results->items[position - 1U];
        }
        --position;
    }
    results->items[position] = *item;

    /* Keep deterministic ordering when scores tie. */
    for (index = 1U; index < results->count; ++index) {
        if (results->items[index - 1U].score == results->items[index].score &&
            strcmp(results->items[index - 1U].title,
                   results->items[index].title) > 0) {
            UmiUiQuickAccessItem temporary = results->items[index - 1U];
            results->items[index - 1U] = results->items[index];
            results->items[index] = temporary;
        }
    }
}

int umi_ui_quick_access_score(const char *query, const char *candidate)
{
    return umi_ui_fuzzy_score_ci(query, candidate);
}

UmiStatus umi_ui_quick_access_search_commands(
    const UmiCommandRegistry *registry,
    const char *query,
    UmiUiQuickAccessResults *out_results)
{
    size_t index;
    if (registry == NULL || query == NULL || out_results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_results, 0, sizeof(*out_results));

    for (index = 0U; index < umi_command_registry_count(registry); ++index) {
        UmiCommandSnapshot command;
        UmiUiQuickAccessItem item = {0};
        int title_score;
        int id_score;
        int category_score;
        if (umi_command_registry_at(registry, index, &command) != UMI_STATUS_OK) {
            continue;
        }
        title_score = umi_ui_fuzzy_score_ci(query, command.title);
        id_score = umi_ui_fuzzy_score_ci(query, command.command_id);
        category_score = umi_ui_fuzzy_score_ci(query, command.category);
        item.score = title_score;
        if (id_score > item.score) item.score = id_score;
        if (category_score >= 0 && category_score + 20 > item.score) {
            item.score = category_score + 20;
        }
        if (query[0] != '\0' && item.score < 0) continue;

        (void)snprintf(item.command_id, sizeof(item.command_id),
                       "%s", command.command_id);
        (void)snprintf(item.title, sizeof(item.title), "%s", command.title);
        (void)snprintf(item.category, sizeof(item.category), "%s", command.category);
        (void)snprintf(item.description, sizeof(item.description),
                       "%s", command.description);
        item.enabled = umi_command_registry_is_enabled(registry,
                                                       command.command_id,
                                                       NULL);
        insert_sorted(out_results, &item);
    }
    return UMI_STATUS_OK;
}
