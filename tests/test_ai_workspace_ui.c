/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_workspace_ui.c
 *
 * PURPOSE:
 *   Verify reusable assistant windows and model-comparison view properties.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This test proves applications can discover the common windows and render a
 * comparison without depending on a particular desktop toolkit.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/umicom.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiWindowCatalogue catalogue;
    UmiAiModelEnsembleReport report;
    UmiUiViewModel *view = NULL;
    UmiUiValue value;

    umi_ui_window_catalogue_init(&catalogue);
    assert(umi_ai_ui_assistant_windows_register(&catalogue) == UMI_STATUS_OK);
    assert(catalogue.count == 6U);
    assert(umi_ui_window_catalogue_find(
               &catalogue, UMI_AI_UI_WINDOW_MODEL_COMPARISON) != NULL);

    assert(umi_ai_model_ensemble_report_initialize(&report) == UMI_STATUS_OK);
    (void)snprintf(
        report.request_id, sizeof(report.request_id), "%s", "request.1");
    report.result_count = 1U;
    report.success_count = 1U;
    report.selected_index = 0U;
    report.revision = 2U;
    report.results[0].structure_size =
        (uint32_t)sizeof(report.results[0]);
    report.results[0].api_version = UMI_AI_MODEL_ENSEMBLE_API_VERSION;
    assert(umi_ai_model_target_initialize(
               &report.results[0].target, "local", "model-a", "Local A", 0) ==
           UMI_STATUS_OK);
    report.results[0].status = UMI_STATUS_OK;
    assert(umi_ai_response_set_text(
               &report.results[0].response, "A useful answer") == UMI_STATUS_OK);

    assert(umi_ai_ui_model_comparison_view_create(
               "comparison", &report, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
               view, "ai-models.success-count", &value) == UMI_STATUS_OK);
    assert(value.integer_value == 1);
    assert(umi_ui_view_model_get_property(
               view, "ai-models.row.0", &value) == UMI_STATUS_OK);
    assert(strstr(value.string_value, "A useful answer") != NULL);
    umi_ui_view_model_destroy(view);
    return 0;
}
