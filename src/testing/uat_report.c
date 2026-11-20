/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/uat_report.c
 *
 * PURPOSE:
 *   Serialize UAT findings through the shared JSON writer, keeping skipped
 *   work, cancellation and cleanup failures visible and output opt-in.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/uat_target.h"
#include "umicom/protocol/json_writer.h"
#include <stdlib.h>

static void WriteTarget(UmiJsonWriter *writer, const UmiUatTargetEvidence *scope)
{
    UmiJsonWriterRaw(writer, ",\"target\":{\"protocol\":");
    UmiJsonWriterUInt64(writer, scope->target.protocol);
    UmiJsonWriterRaw(writer, ",\"platform\":"); UmiJsonWriterUInt64(writer, scope->target.platform);
    UmiJsonWriterRaw(writer, ",\"target_id\":"); UmiJsonWriterString(writer, scope->target.targetId);
    UmiJsonWriterRaw(writer, ",\"session_id\":"); UmiJsonWriterString(writer, scope->target.sessionId);
    UmiJsonWriterRaw(writer, ",\"allowed_operations\":"); UmiJsonWriterUInt64(writer, scope->allowedOperations);
    UmiJsonWriterRaw(writer, ",\"supported_operations\":"); UmiJsonWriterUInt64(writer, scope->supportedOperations);
    UmiJsonWriterRaw(writer, "}");
}

UmiStatus UmiUatReportJson(const UmiUatReport *report, int includeOutput,
    char *buffer, size_t capacity)
{
    UmiUatSummary summary;
    UmiUatTargetEvidence scope;
    UmiUatFinding *finding;
    UmiJsonWriter writer;
    UmiStatus status;
    if (buffer == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    buffer[0] = '\0';
    if (includeOutput != 0 && includeOutput != 1) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiUatReportSummary(report, &summary);
    if (status != UMI_STATUS_OK) return status;
    status = UmiUatReportTarget(report, &scope);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) return status;
    const int scoped = status == UMI_STATUS_OK;
    finding = malloc(sizeof(*finding));
    if (finding == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonWriterInit(&writer, buffer, capacity);
    UmiJsonWriterRaw(&writer, "{\"schema\":1,\"product\":\"Umicom Automated Testing\",\"accepted\":");
    UmiJsonWriterBool(&writer, summary.accepted);
    UmiJsonWriterRaw(&writer, ",\"cancellation_requested\":");
    UmiJsonWriterBool(&writer, summary.cancellationRequested);
    UmiJsonWriterRaw(&writer, ",\"planned\":"); UmiJsonWriterUInt64(&writer, summary.planned);
    UmiJsonWriterRaw(&writer, ",\"attempted\":"); UmiJsonWriterUInt64(&writer, summary.attempted);
    UmiJsonWriterRaw(&writer, ",\"passed\":"); UmiJsonWriterUInt64(&writer, summary.passed);
    UmiJsonWriterRaw(&writer, ",\"failed\":"); UmiJsonWriterUInt64(&writer, summary.failed);
    UmiJsonWriterRaw(&writer, ",\"skipped\":"); UmiJsonWriterUInt64(&writer, summary.skipped);
    UmiJsonWriterRaw(&writer, ",\"not_run\":"); UmiJsonWriterUInt64(&writer, summary.notRun);
    UmiJsonWriterRaw(&writer, ",\"cleanup_failed\":"); UmiJsonWriterUInt64(&writer, summary.cleanupFailed);
    UmiJsonWriterRaw(&writer, ",\"output_included\":"); UmiJsonWriterBool(&writer, includeOutput);
    /* Scoped fields extend the report without changing the established
     * fields consumed by readers of unscoped workflows. */
    if (scoped) WriteTarget(&writer, &scope);
    UmiJsonWriterRaw(&writer, ",\"findings\":[");
    for (size_t i = 0U; i < summary.planned && writer.status == UMI_STATUS_OK; ++i) {
        status = UmiUatReportFinding(report, i, finding);
        if (status != UMI_STATUS_OK) { writer.status = status; break; }
        if (i != 0U) UmiJsonWriterRaw(&writer, ",");
        UmiJsonWriterRaw(&writer, "{\"id\":"); UmiJsonWriterString(&writer, finding->result.test_id);
        UmiJsonWriterRaw(&writer, ",\"phase\":"); UmiJsonWriterInt64(&writer, finding->phase);
        if (scoped) {
            UmiUatOperation operation;
            status = UmiUatReportOperation(report, i, &operation);
            if (status != UMI_STATUS_OK) { writer.status = status; break; }
            UmiJsonWriterRaw(&writer, ",\"operation\":"); UmiJsonWriterUInt64(&writer, operation);
        }
        UmiJsonWriterRaw(&writer, ",\"attempted\":"); UmiJsonWriterBool(&writer, finding->attempted);
        UmiJsonWriterRaw(&writer, ",\"state\":"); UmiJsonWriterString(&writer, umi_test_state_text(finding->result.state));
        UmiJsonWriterRaw(&writer, ",\"status\":"); UmiJsonWriterInt64(&writer, finding->result.status);
        UmiJsonWriterRaw(&writer, ",\"exit_code\":");
        if (finding->attempted) UmiJsonWriterInt64(&writer, finding->result.exit_code);
        else UmiJsonWriterRaw(&writer, "null");
        UmiJsonWriterRaw(&writer, ",\"duration_ms\":");
        if (finding->attempted) UmiJsonWriterUInt64(&writer, finding->result.duration_ms);
        else UmiJsonWriterRaw(&writer, "null");
        if (includeOutput) {
            UmiJsonWriterRaw(&writer, ",\"output\":"); UmiJsonWriterString(&writer, finding->result.output);
        }
        UmiJsonWriterRaw(&writer, "}");
    }
    UmiJsonWriterRaw(&writer, "]}\n");
    status = writer.status;
    free(finding);
    if (status != UMI_STATUS_OK) buffer[0] = '\0';
    return status;
}
