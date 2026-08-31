/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_ui/model_comparison_views.c
 *
 * PURPOSE:
 *   Build a reusable side-by-side model-result view from an ensemble report.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Failed providers remain visible beside successful providers. This helps the
 * user understand whether an answer is missing because of policy, availability
 * or another provider-specific status.
 */
#include "umicom/ai_ui/model_comparison_views.h"

#include <stdio.h>
#include <string.h>

static UmiStatus set_string(UmiUiViewModel *view,
                            const char *key,
                            const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(
        &value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_integer(UmiUiViewModel *view,
                             const char *key,
                             int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

UmiStatus umi_ai_ui_model_comparison_view_create(
    const char *view_id,
    const UmiAiModelEnsembleReport *report,
    UmiUiViewModel **out_view)
{
    size_t index;
    UmiStatus status;

    if (view_id == NULL || report == NULL || out_view == NULL ||
        report->structure_size < sizeof(*report) ||
        report->api_version != UMI_AI_MODEL_ENSEMBLE_API_VERSION ||
        report->result_count > UMI_AI_MODEL_ENSEMBLE_MAX_TARGETS ||
        report->success_count > report->result_count ||
        memchr(report->request_id, '\0', sizeof(report->request_id)) == NULL ||
        (report->selected_index != SIZE_MAX &&
         report->selected_index >= report->result_count)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_view = NULL;
    status = umi_ui_view_model_create(
        view_id, "umicom.ai-model-comparison", UMI_UI_ROLE_PANE, out_view);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "umicom.view-kind", "ai-model-comparison");
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "title", "Model Comparison");
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "summary",
        "Compare approved local and remote model answers to the same request.");
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-models.request-id", report->request_id);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-models.result-count", (int64_t)report->result_count);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-models.success-count", (int64_t)report->success_count);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-models.selected-index",
        report->selected_index < report->result_count
            ? (int64_t)report->selected_index : -1);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-models.revision", (int64_t)report->revision);

    for (index = 0U; status == UMI_STATUS_OK &&
         index < report->result_count; ++index) {
        const UmiAiModelResult *result = &report->results[index];
        char key[96];
        char row[UMI_UI_VALUE_STRING_CAPACITY];

        /* Reject malformed external reports before any bounded array is read. */
        if (result->structure_size < sizeof(*result) ||
            result->api_version != UMI_AI_MODEL_ENSEMBLE_API_VERSION ||
            result->target.structure_size < sizeof(result->target) ||
            result->target.api_version !=
                UMI_AI_MODEL_ENSEMBLE_API_VERSION ||
            memchr(result->target.label, '\0',
                   sizeof(result->target.label)) == NULL ||
            memchr(result->target.provider_id, '\0',
                   sizeof(result->target.provider_id)) == NULL ||
            memchr(result->target.model_id, '\0',
                   sizeof(result->target.model_id)) == NULL ||
            (result->status == UMI_STATUS_OK &&
             memchr(result->response.text, '\0',
                    sizeof(result->response.text)) == NULL)) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            break;
        }

        /* One compact row is readable in panes and remains easy to test. */
        (void)snprintf(key, sizeof(key), "ai-models.row.%zu", index);
        (void)snprintf(
            row, sizeof(row), "%.96s | %.96s | %.96s | %s | %.180s",
            result->target.label, result->target.provider_id,
            result->target.model_id, umi_status_text(result->status),
            result->status == UMI_STATUS_OK ? result->response.text : "");
        status = set_string(*out_view, key, row);
    }
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}
