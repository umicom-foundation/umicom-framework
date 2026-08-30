/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/views/validation.c
 *
 * PURPOSE:
 *   Project coding-agent build/test validation evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/views/validation.h"

#include <stdio.h>

#include "umicom/ai_developer_experience/action_ids.h"

UmiStatus umi_ai_developer_validation_view_create(
    const char *view_id,
    const UmiAiCodingValidationReport *report,
    size_t visible_rows,
    UmiUiViewModel **out_view)
{
    size_t count;
    size_t index;
    UmiStatus status;

    if (report == NULL || visible_rows == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (visible_rows > UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY) {
        visible_rows = UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY;
    }

    status = umi_ai_developer_view_create_base(
        view_id,
        "ai-developer-validation",
        "AI Validation",
        "Build, test, lint and package evidence used to accept, repair or roll back AI changes.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    count =
        report->result_count > visible_rows
            ? visible_rows
            : report->result_count;

    status = umi_ai_developer_view_set_boolean(
        *out_view, "ai-validation.passed", report->passed);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view, "ai-validation.row-count", (int64_t)count);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-validation.failed-count",
            (int64_t)report->failed_count);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-validation.required-failures",
            (int64_t)report->required_failed_count);

    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const UmiAiCodingValidationResult *result = &report->results[index];
        char key[96];
        char row[UMI_UI_VALUE_STRING_CAPACITY];

        (void)snprintf(key, sizeof(key), "ai-validation.row.%zu", index);
        (void)snprintf(
            row,
            sizeof(row),
            "%s | %s | exit %d | %llu ms | %.320s",
            result->step_id,
            result->passed ? "passed" : "failed",
            result->exit_code,
            (unsigned long long)result->duration_ms,
            result->output);

        status = umi_ai_developer_view_set_string(*out_view, key, row);
    }

    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 0U,
            umi_ai_developer_action_open_tasks(),
            "Tasks",
            "Return to AI tasks",
            1);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 1U,
            umi_ai_developer_action_open_review(),
            "Patch Review",
            "Inspect the patch associated with validation",
            1);

    return status;
}
