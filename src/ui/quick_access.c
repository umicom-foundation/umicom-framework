/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/quick_access.c
 *
 * PURPOSE:
 *   Implement deterministic fuzzy ranking over the Framework command registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The command registry remains the source of truth. Quick Access creates temporary ranked snapshots and never owns command handlers.
 */

#include "umicom/ui/quick_access.h"

#include <stdio.h>
#include <string.h>

#include "workbench_service_internal.h"

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static void insert_sorted(UmiUiQuickAccessResults *results,
                          const UmiUiQuickAccessItem *item)
{
    size_t position = 0U;
    size_t move;

    while (position < results->count) {
        const UmiUiQuickAccessItem *existing = &results->items[position];
        if (item->score > existing->score ||
            (item->score == existing->score &&
             strcmp(item->title, existing->title) < 0) ||
            (item->score == existing->score &&
             strcmp(item->title, existing->title) == 0 &&
             strcmp(item->action_id, existing->action_id) < 0)) {
            break;
        }
        ++position;
    }
    if (results->count >= UMI_UI_QUICK_ACCESS_MAX_RESULTS &&
        position >= UMI_UI_QUICK_ACCESS_MAX_RESULTS) {
        return;
    }
    if (results->count < UMI_UI_QUICK_ACCESS_MAX_RESULTS) {
        ++results->count;
    }
    for (move = results->count - 1U; move > position; --move) {
        results->items[move] = results->items[move - 1U];
    }
    results->items[position] = *item;
}

static int best_score(const char *query,
                      const char *first,
                      const char *second,
                      const char *third)
{
    int score = umi_ui_fuzzy_score_ci(query, first);
    int candidate = umi_ui_fuzzy_score_ci(query, second);
    if (candidate > score) score = candidate;
    candidate = umi_ui_fuzzy_score_ci(query, third);
    if (candidate > score) score = candidate;
    return score;
}

static int command_has_visible_action(const UmiUiActionModel *actions,
                                      const char *command_id)
{
    size_t index;
    for (index = 0U; index < umi_ui_action_model_count(actions); ++index) {
        UmiUiActionSnapshot action;
        if (umi_ui_action_model_at(actions, index, &action) == UMI_STATUS_OK &&
            action.visible && strcmp(action.command_id, command_id) == 0) {
            return 1;
        }
    }
    return 0;
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

UmiStatus umi_ui_quick_access_search_actions(
    const UmiCommandRegistry *registry,
    const UmiUiActionModel *actions,
    const char *query,
    UmiUiQuickAccessResults *out_results)
{
    size_t index;
    if (registry == NULL || actions == NULL || query == NULL ||
        out_results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_results, 0, sizeof(*out_results));

    for (index = 0U; index < umi_ui_action_model_count(actions); ++index) {
        UmiUiActionSnapshot action;
        UmiCommandSnapshot command;
        UmiUiQuickAccessItem item = {0};
        int command_score;
        if (umi_ui_action_model_at(actions, index, &action) != UMI_STATUS_OK ||
            !action.visible ||
            umi_command_registry_snapshot(registry, action.command_id,
                                          &command) != UMI_STATUS_OK) {
            continue;
        }
        item.score = best_score(query, action.label, action.action_id,
                                action.tooltip);
        command_score = best_score(query, command.title, command.command_id,
                                   command.category);
        if (command_score >= 0 && command_score + 12 > item.score) {
            item.score = command_score + 12;
        }
        if (query[0] != '\0' && item.score < 0) continue;

        copy_text(item.action_id, sizeof(item.action_id), action.action_id);
        copy_text(item.command_id, sizeof(item.command_id), command.command_id);
        copy_text(item.title, sizeof(item.title), action.label);
        copy_text(item.category, sizeof(item.category), command.category);
        copy_text(item.description, sizeof(item.description),
                  action.tooltip[0] != '\0' ? action.tooltip
                                             : command.description);
        copy_text(item.icon_name, sizeof(item.icon_name), action.icon_name);
        copy_text(item.accelerator, sizeof(item.accelerator),
                  action.accelerator);
        item.enabled = action.enabled && umi_command_registry_is_enabled(
            registry, command.command_id,
            action.argument[0] != '\0' ? action.argument : NULL);
        item.checkable = action.checkable;
        item.checked = action.checked;
        insert_sorted(out_results, &item);
    }

    /* Commands without a visible action remain discoverable for extensions,
     * automation and advanced workflows. */
    for (index = 0U; index < umi_command_registry_count(registry); ++index) {
        UmiCommandSnapshot command;
        UmiUiQuickAccessItem item = {0};
        if (umi_command_registry_at(registry, index, &command) != UMI_STATUS_OK ||
            command_has_visible_action(actions, command.command_id)) {
            continue;
        }
        item.score = best_score(query, command.title, command.command_id,
                                command.category);
        if (query[0] != '\0' && item.score < 0) continue;
        (void)snprintf(item.command_id, sizeof(item.command_id), "%s",
                       command.command_id);
        (void)snprintf(item.title, sizeof(item.title), "%s", command.title);
        (void)snprintf(item.category, sizeof(item.category), "%s",
                       command.category);
        (void)snprintf(item.description, sizeof(item.description), "%s",
                       command.description);
        item.enabled = umi_command_registry_is_enabled(registry,
                                                       command.command_id,
                                                       NULL);
        insert_sorted(out_results, &item);
    }
    return UMI_STATUS_OK;
}
