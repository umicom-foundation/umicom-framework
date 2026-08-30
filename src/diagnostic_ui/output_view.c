/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostic_ui/output_view.c
 *
 * PURPOSE:
 *   Implement the output view behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostic_ui/output_view.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static UmiStatus output_set_string(UmiUiViewModel *view, const char *key, const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(&value, text);
    return status == UMI_STATUS_OK ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus output_set_integer(UmiUiViewModel *view, const char *key, uint64_t number)
{
    UmiUiValue value;
    int64_t bounded = number <= (uint64_t)INT64_MAX ? (int64_t)number : INT64_MAX;
    UmiStatus status = umi_ui_value_set_integer(&value, bounded);
    return status == UMI_STATUS_OK ? umi_ui_view_model_set_property(view, key, &value) : status;
}

UmiStatus umi_diagnostic_output_view_create(const char *view_id,
                                            UmiDiagnosticPipeline *pipeline,
                                            const char *channel_id,
                                            UmiUiViewModel **out_view)
{
    UmiOutputRecord rows[UMI_DIAGNOSTIC_OUTPUT_VIEW_ROW_MAX];
    UmiOutputBuffer *buffer;
    UmiOutputBufferSummary summary;
    UmiUiViewModel *view;
    size_t retained = 0U;
    size_t matched = 0U;
    size_t count;
    size_t index;
    UmiStatus status;
    if (view_id == NULL || pipeline == NULL || out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_view = NULL;
    buffer = umi_diagnostic_pipeline_output(pipeline);
    status = umi_output_buffer_summary(buffer, &summary);
    count = umi_output_buffer_count(buffer);
    for (index = 0U; index < count && status == UMI_STATUS_OK; ++index) {
        UmiOutputRecord record;
        status = umi_output_buffer_at(buffer, index, &record);
        if (status == UMI_STATUS_OK &&
            (channel_id == NULL || channel_id[0] == '\0' || strcmp(channel_id, record.channel_id) == 0)) {
            rows[matched % UMI_DIAGNOSTIC_OUTPUT_VIEW_ROW_MAX] = record;
            ++matched;
            if (retained < UMI_DIAGNOSTIC_OUTPUT_VIEW_ROW_MAX) ++retained;
        }
    }
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_view_model_create(view_id, "umicom.diagnostics.output",
                                      UMI_UI_ROLE_PANE, &view);
    if (status != UMI_STATUS_OK) return status;
    if (status == UMI_STATUS_OK) status = output_set_string(view, "umicom.view-kind", "output");
    if (status == UMI_STATUS_OK) status = output_set_string(view, "title", "Output");
    if (status == UMI_STATUS_OK) status = output_set_string(view, "output.channel",
        channel_id != NULL && channel_id[0] != '\0' ? channel_id : "all");
    if (status == UMI_STATUS_OK) status = output_set_integer(view, "output.count", retained);
    if (status == UMI_STATUS_OK) status = output_set_integer(view, "output.total", matched);
    if (status == UMI_STATUS_OK) status = output_set_integer(view, "output.channels", summary.channel_count);
    for (index = 0U; index < retained && status == UMI_STATUS_OK; ++index) {
        size_t first = matched > retained ? matched - retained : 0U;
        const UmiOutputRecord *record = &rows[(first + index) % UMI_DIAGNOSTIC_OUTPUT_VIEW_ROW_MAX];
        char key[64];
        char row[UMI_UI_VALUE_STRING_CAPACITY];
        (void)snprintf(key, sizeof(key), "output.row.%zu", index);
        (void)snprintf(row, sizeof(row), "[%.32s] [%.16s] %.440s",
                       record->channel_id, umi_output_stream_text(record->stream), record->text);
        status = output_set_string(view, key, row);
    }
    if (status != UMI_STATUS_OK) {
        umi_ui_view_model_destroy(view);
        return status;
    }
    *out_view = view;
    return UMI_STATUS_OK;
}
