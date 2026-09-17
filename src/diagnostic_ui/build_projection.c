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

/* Provide the build severity operation used by this module and its client applications. */
static UmiDiagnosticSeverity build_severity(UmiBuildDiagnosticSeverity severity)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (severity) {
        case UMI_BUILD_DIAGNOSTIC_NOTE: return UMI_DIAGNOSTIC_INFO;
        case UMI_BUILD_DIAGNOSTIC_WARNING: return UMI_DIAGNOSTIC_WARNING;
        case UMI_BUILD_DIAGNOSTIC_ERROR: return UMI_DIAGNOSTIC_ERROR;
        case UMI_BUILD_DIAGNOSTIC_FATAL: return UMI_DIAGNOSTIC_FATAL;
        default: return UMI_DIAGNOSTIC_INFO;
    }
}

/*
 * Provide the ingest output lines operation used by this module and its client
 * applications.
 */
static UmiStatus ingest_output_lines(UmiDiagnosticPipeline *pipeline,
                                     const UmiBuildResult *result,
                                     const char *source)
{
    const char *cursor = result->output;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        char line[UMI_OUTPUT_TEXT_CAPACITY];
        size_t length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        UmiStatus status;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(line)) length = sizeof(line) - 1U;
        (void)memcpy(line, cursor, length);
        line[length] = '\0';
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (result->diagnostics.count == 0U) {
            status = umi_diagnostic_pipeline_ingest_line(
                pipeline, "build", "Build", source,
                result->status == UMI_STATUS_OK ? UMI_OUTPUT_STREAM_STANDARD : UMI_OUTPUT_STREAM_ERROR,
                line, result->operation_id);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            UmiOutputRecord record;
            status = umi_output_record_init(
                &record, "build", "Build", source,
                result->status == UMI_STATUS_OK ? UMI_OUTPUT_STREAM_STANDARD : UMI_OUTPUT_STREAM_ERROR,
                line);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) {
                record.correlation_id = result->operation_id;
                record.timestamp_ns = result->started_ns;
                status = umi_output_buffer_append(
                    umi_diagnostic_pipeline_output(pipeline), &record);
            }
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) break;
        cursor = end + 1;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic build result ingest operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_build_result_ingest(UmiDiagnosticPipeline *pipeline,
                                             const UmiBuildResult *result,
                                             const char *source)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (result->diagnostics.count > UMI_BUILD_MAX_DIAGNOSTICS ||
        memchr(result->output, '\0', sizeof result->output) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < result->diagnostics.count; ++i) {
        const UmiBuildDiagnostic *item = &result->diagnostics.items[i];
        if (memchr(item->file, '\0', sizeof item->file) == NULL ||
            memchr(item->code, '\0', sizeof item->code) == NULL ||
            memchr(item->message, '\0', sizeof item->message) == NULL ||
            item->severity < UMI_BUILD_DIAGNOSTIC_NOTE || item->severity > UMI_BUILD_DIAGNOSTIC_FATAL)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    source = source != NULL && source[0] != '\0' ? source : "build";
    /* Replace active compiler findings from the same build producer only when
     * a new compile result arrives. Other tools' findings remain independent. */
    if (result->phase == UMI_BUILD_PHASE_BUILD) {
        UmiDiagnosticModel *model = umi_diagnostic_pipeline_model(pipeline);
        size_t count = umi_diagnostic_model_count(model);
        for (size_t i = 0U; i < count; ++i) {
            UmiDiagnosticSnapshot old;
            status = umi_diagnostic_model_at(model, i, &old);
            if (status != UMI_STATUS_OK) return status;
            if (old.kind == UMI_DIAGNOSTIC_KIND_COMPILER && !old.resolved &&
                old.correlation_id != result->operation_id && strcmp(old.source, source) == 0 &&
                strncmp(old.id, "build-", 6U) == 0) {
                status = umi_diagnostic_model_resolve(model, old.id, 1);
                if (status != UMI_STATUS_OK) return status;
            }
        }
    }
    status = ingest_output_lines(pipeline, result, source);
    /* Visit each bounded item once so every record receives the same rule. */
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
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(diagnostic.code, sizeof(diagnostic.code), "%s", item->code);
        if (strlen(item->file) < sizeof diagnostic.uri &&
            item->line <= UINT32_MAX && item->column <= UINT32_MAX) {
            strcpy(diagnostic.uri, item->file);
            diagnostic.line = (uint32_t)item->line;
            diagnostic.column = (uint32_t)item->column;
        } else {
            /* Keep the diagnostic message, but do not offer a cropped location. */
            strcpy(diagnostic.detail, "The source location exceeds the Problems record capacity. Consult the complete build output.");
        }
        diagnostic.correlation_id = result->operation_id;
        diagnostic.timestamp_ns = result->started_ns;
        status = umi_diagnostic_pipeline_ingest_diagnostic(pipeline, &diagnostic);
    }
    return status;
}
