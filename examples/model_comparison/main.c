/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/model_comparison/main.c
 *
 * PURPOSE:
 *   Teach checked selection of captured model responses through installed Framework APIs without provider effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/model_ensemble.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    UmiAiModelEnsembleReport report;
    if (umi_ai_model_ensemble_report_initialize(&report) != UMI_STATUS_OK) return 1;
    memcpy(report.request_id, "synthetic-comparison", 21U);
    report.result_count = 2U; report.success_count = 1U; report.selected_index = SIZE_MAX;
    for (size_t i = 0U; i < report.result_count; ++i) {
        UmiAiModelResult *result = &report.results[i];
        result->structure_size = (uint32_t)sizeof *result;
        result->api_version = UMI_AI_MODEL_ENSEMBLE_API_VERSION;
        if (umi_ai_model_target_initialize(&result->target, "example-observation",
                i == 0U ? "model-a" : "model-b", i == 0U ? "First capture" : "Unavailable capture", 0) != UMI_STATUS_OK) return 1;
        umi_ai_response_init(&result->response);
        result->status = i == 0U ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
        if (i == 0U && umi_ai_response_set_text(&result->response, "A synthetic recorded answer") != UMI_STATUS_OK) return 1;
    }
    /* This is captured teaching data, not evidence of an inference. Selection
     * does not call a provider or assess whether the response is correct. */
    if (UmiAiModelEnsembleReportValidate(&report) != UMI_STATUS_OK ||
        umi_ai_model_ensemble_select(&report, 0U) != UMI_STATUS_OK) return 1;
    const UmiAiModelResult *selected = umi_ai_model_ensemble_selected(&report);
    if (selected == NULL || report.revision != 1U) return 1;
    if (printf("Selected %s; retained failure: %s\n", selected->target.model_id,
            umi_status_text(report.results[1].status)) < 0) return 1;
    return fflush(stdout) == 0 ? 0 : 1;
}
