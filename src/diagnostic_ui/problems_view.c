#include "umicom/diagnostic_ui/problems_view.h"

#include <inttypes.h>
#include <stdio.h>

#include "umicom/diagnostics/query.h"

static UmiStatus set_string(UmiUiViewModel *view, const char *key, const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(&value, text);
    return status == UMI_STATUS_OK ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_integer(UmiUiViewModel *view, const char *key, uint64_t number)
{
    UmiUiValue value;
    int64_t bounded = number <= (uint64_t)INT64_MAX ? (int64_t)number : INT64_MAX;
    UmiStatus status = umi_ui_value_set_integer(&value, bounded);
    return status == UMI_STATUS_OK ? umi_ui_view_model_set_property(view, key, &value) : status;
}

UmiStatus umi_diagnostic_problems_view_create(const char *view_id,
                                              UmiDiagnosticPipeline *pipeline,
                                              UmiUiViewModel **out_view)
{
    UmiDiagnosticModelSummary summary;
    UmiDiagnosticQuery query;
    UmiDiagnosticQueryResult result;
    UmiUiViewModel *view;
    UmiStatus status;
    size_t index;
    if (view_id == NULL || pipeline == NULL || out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_view = NULL;
    status = umi_ui_view_model_create(view_id, "umicom.diagnostics.problems",
                                      UMI_UI_ROLE_PANE, &view);
    if (status != UMI_STATUS_OK) return status;
    status = umi_diagnostic_model_summary(umi_diagnostic_pipeline_model(pipeline), &summary);
    umi_diagnostic_query_init(&query);
    query.sort = UMI_DIAGNOSTIC_SORT_SEVERITY_DESCENDING;
    query.limit = UMI_DIAGNOSTIC_PROBLEMS_VIEW_ROW_MAX;
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_query_execute(umi_diagnostic_pipeline_model(pipeline),
                                              &query, &result);
    }
    if (status == UMI_STATUS_OK) status = set_string(view, "umicom.view-kind", "problems");
    if (status == UMI_STATUS_OK) status = set_string(view, "title", "Problems");
    if (status == UMI_STATUS_OK) status = set_integer(view, "problem.count", result.count);
    if (status == UMI_STATUS_OK) status = set_integer(view, "problem.total", summary.retained_count);
    if (status == UMI_STATUS_OK) status = set_integer(view, "problem.active", summary.active_count);
    if (status == UMI_STATUS_OK) status = set_integer(view, "problem.errors",
        summary.by_severity[UMI_DIAGNOSTIC_ERROR] + summary.by_severity[UMI_DIAGNOSTIC_FATAL]);
    if (status == UMI_STATUS_OK) status = set_integer(view, "problem.warnings",
        summary.by_severity[UMI_DIAGNOSTIC_WARNING]);
    for (index = 0U; index < result.count && status == UMI_STATUS_OK; ++index) {
        char key[64];
        char row[UMI_UI_VALUE_STRING_CAPACITY];
        const UmiDiagnosticSnapshot *item = &result.items[index];
        (void)snprintf(key, sizeof(key), "problem.row.%zu", index);
        (void)snprintf(row, sizeof(row), "%s\t%.80s\t%.300s\t%.80s:%" PRIu32 ":%" PRIu32,
                       umi_diagnostic_severity_text(item->severity), item->code,
                       item->message, item->uri, item->line, item->column);
        status = set_string(view, key, row);
    }
    if (status != UMI_STATUS_OK) {
        umi_ui_view_model_destroy(view);
        return status;
    }
    *out_view = view;
    return UMI_STATUS_OK;
}
