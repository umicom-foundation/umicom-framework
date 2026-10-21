/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/evidence_text.c
 * PURPOSE: Keep selected-test reporting and retention disclosures in Framework.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/evidence_text.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct EvidenceTextWriter {
    char *data;
    size_t bytes, capacity;
    UmiStatus status;
} EvidenceTextWriter;

/* Measure before allocating. Both passes consume the same immutable capture,
 * so allocation failure cannot expose a partial report to a host application. */
static void Append(EvidenceTextWriter *writer, const char *format, ...)
{
    if (writer->status != UMI_STATUS_OK) return;
    va_list args;
    va_start(args, format);
    int count = writer->data == NULL ? vsnprintf(NULL, 0U, format, args) :
        vsnprintf(writer->data + writer->bytes, writer->capacity - writer->bytes, format, args);
    va_end(args);
    if (count < 0) { writer->status = UMI_STATUS_INTERNAL_ERROR; return; }
    size_t added = (size_t)count;
    if (added > UMI_TEST_EVIDENCE_TEXT_MAX_BYTES - writer->bytes ||
        (writer->data != NULL && added >= writer->capacity - writer->bytes)) {
        writer->status = UMI_STATUS_CAPACITY_EXCEEDED; return;
    }
    writer->bytes += added;
}
static UmiStatus Render(const UmiTestEvidence *evidence, EvidenceTextWriter *writer)
{
    UmiTestEvidenceSummary summary;
    UmiStatus status = UmiTestEvidenceGetSummary(evidence, &summary);
    if (status != UMI_STATUS_OK) return status;
    Append(writer, "Selected test: %s\nRetained results: %zu of %zu | output records: %zu of %zu\n"
        "Newest first. Session labels distinguish separate runs.\n",
        summary.item_id, summary.retained_results, summary.matching_results,
        summary.retained_output, summary.matching_output);
    if (summary.session_id[0] != '\0') Append(writer, "Session filter: %s\n", summary.session_id);
    Append(writer, "\n");
    if (summary.retained_results == 0U)
        Append(writer, "No result is retained for this test. This is not a pass.\n");
    for (size_t i = 0U; writer->status == UMI_STATUS_OK && i < summary.retained_results; ++i) {
        UmiTestPlatformResultSnapshot row;
        status = UmiTestEvidenceResultAt(evidence, i, &row);
        if (status != UMI_STATUS_OK) return status;
        Append(writer, "[%s] %s | %.2f ms | exit %d%s\n%s\n%s\n\n",
            row.session_id[0] != '\0' ? row.session_id : "session unspecified",
            umi_test_platform_outcome_text((UmiTestPlatformOutcome)row.outcome),
            row.duration_ms, row.exit_code, row.flaky ? " | flaky" : "", row.message, row.failure_details);
    }
    Append(writer, "OUTPUT\n");
    if (summary.retained_output == 0U) Append(writer, "No output retained for this test.\n");
    for (size_t i = 0U; writer->status == UMI_STATUS_OK && i < summary.retained_output; ++i) {
        UmiTestPlatformOutputSnapshot row;
        status = UmiTestEvidenceOutputAt(evidence, i, &row);
        if (status != UMI_STATUS_OK) return status;
        Append(writer, "[%s / %s] %s\n", row.session_id[0] != '\0' ? row.session_id :
            "session unspecified", row.stream, row.text);
    }
    return writer->status;
}
UmiStatus UmiTestEvidenceFormatText(const UmiTestEvidence *evidence, char **outText, size_t *outBytes)
{
    if (outText == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outText = NULL;
    if (evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    EvidenceTextWriter measure = {0};
    measure.status = UMI_STATUS_OK;
    UmiStatus status = Render(evidence, &measure);
    if (status != UMI_STATUS_OK) return status;
    char *text = malloc(measure.bytes + 1U);
    if (text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    EvidenceTextWriter writer = {text, 0U, measure.bytes + 1U, UMI_STATUS_OK};
    status = Render(evidence, &writer);
    if (status == UMI_STATUS_OK && writer.bytes != measure.bytes) status = UMI_STATUS_INTERNAL_ERROR;
    if (status != UMI_STATUS_OK) { free(text); return status; }
    *outText = text;
    if (outBytes != NULL) *outBytes = writer.bytes;
    return UMI_STATUS_OK;
}
void UmiTestEvidenceTextFree(char *text) { free(text); }
