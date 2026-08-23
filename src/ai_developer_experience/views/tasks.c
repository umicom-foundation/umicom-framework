/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/views/tasks.c
 *
 * PURPOSE:
 *   Project recent AI coding task summaries into a toolkit-neutral task list.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/views/tasks.h"

#include <stdio.h>
#include <string.h>

#include "umicom/ai_developer_experience/action_ids.h"

UmiStatus umi_ai_developer_tasks_view_create(
    const char *view_id,
    const UmiAiDeveloperTaskRegistry *tasks,
    const char *active_task_id,
    size_t visible_rows,
    UmiUiViewModel **out_view)
{
    size_t total;
    size_t count;
    size_t first;
    size_t index;
    UmiStatus status;

    if (tasks == NULL || active_task_id == NULL || visible_rows == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (visible_rows > UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY) {
        visible_rows = UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY;
    }

    status = umi_ai_developer_view_create_base(
        view_id,
        "ai-developer-tasks",
        "AI Tasks",
        "Coding-agent task state, iterations, patch size and validation failures.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    total = umi_ai_developer_task_registry_count(tasks);
    count = total > visible_rows ? visible_rows : total;
    first = total - count;

    status = umi_ai_developer_view_set_integer(
        *out_view, "ai-tasks.total-count", (int64_t)total);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view, "ai-tasks.row-count", (int64_t)count);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_string(
            *out_view, "ai-tasks.active-id", active_task_id);

    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiAiDeveloperTaskEntry entry;
        char key[96];
        char row[UMI_UI_VALUE_STRING_CAPACITY];
        int active;

        status = umi_ai_developer_task_registry_at(
            tasks, first + index, &entry);
        if (status != UMI_STATUS_OK) break;

        active =
            active_task_id[0] != '\0' &&
            strcmp(active_task_id, entry.task_id) == 0;

        (void)snprintf(key, sizeof(key), "ai-tasks.row.%zu", index);
        (void)snprintf(
            row,
            sizeof(row),
            "%s%s | %.180s | %s | iteration %u/%u | files %zu | validation failures %zu",
            active ? "* " : "",
            entry.task_id,
            entry.title,
            umi_ai_developer_task_state_text(entry.state),
            entry.iteration,
            entry.maximum_iterations,
            entry.patch_files,
            entry.validation_failures);

        status = umi_ai_developer_view_set_string(*out_view, key, row);
    }

    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 0U,
            umi_ai_developer_action_open_overview(),
            "Overview",
            "Return to AI Developer overview",
            1);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 1U,
            umi_ai_developer_action_open_review(),
            "Patch Review",
            "Open the current patch review",
            active_task_id[0] != '\0');
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 2U,
            umi_ai_developer_action_open_validation(),
            "Validation",
            "Inspect validation evidence",
            active_task_id[0] != '\0');

    return status;
}
