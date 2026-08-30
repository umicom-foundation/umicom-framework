/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostic_ui/build_projection.c
 *
 * PURPOSE:
 *   Implement the build projection behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostic_ui/build_projection.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static UmiDiagnosticSeverity build_severity(UmiBuildDiagnosticSeverity severity)
{
    switch (severity) {
        case UMI_BUILD_DIAGNOSTIC_NOTE: return UMI_DIAGNOSTIC_INFO;
        case UMI_BUILD_DIAGNOSTIC_WARNING: return UMI_DIAGNOSTIC_WARNING;
        case UMI_BUILD_DIAGNOSTIC_ERROR: return UMI_DIAGNOSTIC_ERROR;
        case UMI_BUILD_DIAGNOSTIC_FATAL: return UMI_DIAGNOSTIC_FATAL;
        default: return UMI_DIAGNOSTIC_INFO;
    }
}

static UmiStatus ingest_output_lines(UmiDiagnosticPipeline *pipeline,
                                     const UmiBuildResult *result,
                                     const char *source)
{
    const char *cursor = result->output;
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        char line[UMI_OUTPUT_TEXT_CAPACITY];
        size_t length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        UmiStatus status;
        if (length >= sizeof(line)) length = sizeof(line) - 1U;
        (void)memcpy(line, cursor, length);
        line[length] = '\0';
        if (result->diagnostics.count == 0U) {
            status = umi_diagnostic_pipeline_ingest_line(
                pipeline, "build", "Build", source,
                result->status == UMI_STATUS_OK ? UMI_OUTPUT_STREAM_STANDARD : UMI_OUTPUT_STREAM_ERROR,
                line, result->operation_id);
        } else {
            UmiOutputRecord record;
            status = umi_output_record_init(
                &record, "build", "Build", source,
                result->status == UMI_STATUS_OK ? UMI_OUTPUT_STREAM_STANDARD : UMI_OUTPUT_STREAM_ERROR,
                line);
            if (status == UMI_STATUS_OK) {
                record.correlation_id = result->operation_id;
                record.timestamp_ns = result->started_ns;
                status = umi_output_buffer_append(
                    umi_diagnostic_pipeline_output(pipeline), &record);
            }
        }
        if (status != UMI_STATUS_OK) return status;
        if (end == NULL) break;
        cursor = end + 1;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_build_result_ingest(UmiDiagnosticPipeline *pipeline,
                                             const UmiBuildResult *result,
                                             const char *source)
{
    size_t index;
    UmiStatus status;
    if (pipeline == NULL || result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    source = source != NULL && source[0] != '\0' ? source : "build";
    status = ingest_output_lines(pipeline, result, source);
    for (index = 0U; index < result->diagnostics.count && status == UMI_STATUS_OK; ++index) {
        const UmiBuildDiagnostic *item = &result->diagnostics.items[index];
        UmiDiagnosticSnapshot diagnostic;
        char identifier[UMI_DIAGNOSTIC_ID_CAPACITY];
        (void)snprintf(identifier, sizeof(identifier), "build-%" PRIu64 "-%zu",
                       result->operation_id, index);
        status = umi_diagnostic_snapshot_init(&diagnostic, identifier,
                                              build_severity(item->severity),
                                              UMI_DIAGNOSTIC_KIND_COMPILER,
                                              source, item->message);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(diagnostic.code, sizeof(diagnostic.code), "%s", item->code);
        (void)snprintf(diagnostic.uri, sizeof(diagnostic.uri), "%.*s",
                       (int)(sizeof(diagnostic.uri) - 1U), item->file);
        diagnostic.line = item->line <= UINT32_MAX ? (uint32_t)item->line : UINT32_MAX;
        diagnostic.column = item->column <= UINT32_MAX ? (uint32_t)item->column : UINT32_MAX;
        diagnostic.correlation_id = result->operation_id;
        diagnostic.timestamp_ns = result->started_ns;
        status = umi_diagnostic_pipeline_ingest_diagnostic(pipeline, &diagnostic);
    }
    return status;
}
