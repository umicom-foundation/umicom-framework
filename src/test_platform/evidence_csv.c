/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/evidence_csv.c
 * PURPOSE: Export owned selected-test results and output with honest retention metadata.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/evidence_csv.h"

UmiStatus UmiTestEvidenceExportCsv(const UmiTestEvidence *evidence, UmiCsvDocument **outDocument)
{
    if (outDocument == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outDocument = NULL;
    UmiTestEvidenceSummary summary;
    UmiStatus status = UmiTestEvidenceGetSummary(evidence, &summary);
    if (status != UMI_STATUS_OK) return status;
    UmiCsvDocument *document = NULL;
    status = UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES, &document);
    if (status != UMI_STATUS_OK) return status;
    static const char *const names[] = {"record", "item_id", "session_filter", "result_revision",
        "output_revision", "matching_results", "retained_results", "matching_output", "retained_output",
        "record_id", "session_id", "sequence", "timestamp", "record_revision", "outcome", "duration_ms",
        "exit_code", "flaky", "message", "failure_details", "stream", "output"};
    UmiCsvCell cells[sizeof(names) / sizeof(names[0])];
    const size_t count = sizeof(cells) / sizeof(cells[0]);
    for (size_t i = 0U; i < count; ++i) cells[i] = UmiCsvText(names[i]);
    status = UmiCsvDocumentAppendRow(document, cells, count);
    for (size_t i = 0U; i < count; ++i) cells[i] = UmiCsvText("");
    cells[0] = UmiCsvText("evidence-summary");
    cells[1] = UmiCsvText(summary.item_id);
    cells[2] = UmiCsvText(summary.session_id);
    cells[3] = UmiCsvUnsigned(summary.result_revision);
    cells[4] = UmiCsvUnsigned(summary.output_revision);
    cells[5] = UmiCsvUnsigned(summary.matching_results);
    cells[6] = UmiCsvUnsigned(summary.retained_results);
    cells[7] = UmiCsvUnsigned(summary.matching_output);
    cells[8] = UmiCsvUnsigned(summary.retained_output);
    if (status == UMI_STATUS_OK) status = UmiCsvDocumentAppendRow(document, cells, count);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < summary.retained_results; ++i) {
        UmiTestPlatformResultSnapshot result;
        status = UmiTestEvidenceResultAt(evidence, i, &result);
        if (status != UMI_STATUS_OK) break;
        cells[0] = UmiCsvText("result");
        cells[9] = UmiCsvText(result.id);
        cells[10] = UmiCsvText(result.session_id);
        cells[11] = UmiCsvUnsigned(result.sequence);
        cells[13] = UmiCsvUnsigned(result.revision);
        cells[14] = UmiCsvText(umi_test_platform_outcome_text((UmiTestPlatformOutcome)result.outcome));
        cells[15] = UmiCsvReal(result.duration_ms);
        cells[16] = UmiCsvSigned(result.exit_code);
        cells[17] = UmiCsvSigned(result.flaky);
        cells[18] = UmiCsvText(result.message);
        cells[19] = UmiCsvText(result.failure_details);
        status = UmiCsvDocumentAppendRow(document, cells, count);
    }
    /* Result pointers belong to the preceding loop's copies. Clear them
     * before publishing output rows, including the zero-result case. */
    for (size_t i = 9U; i < count; ++i) cells[i] = UmiCsvText("");
    for (size_t i = 0U; status == UMI_STATUS_OK && i < summary.retained_output; ++i) {
        UmiTestPlatformOutputSnapshot output;
        status = UmiTestEvidenceOutputAt(evidence, i, &output);
        if (status != UMI_STATUS_OK) break;
        cells[0] = UmiCsvText("output");
        cells[9] = UmiCsvText(output.id);
        cells[10] = UmiCsvText(output.session_id);
        cells[12] = UmiCsvUnsigned(output.timestamp);
        cells[13] = UmiCsvUnsigned(output.revision);
        cells[20] = UmiCsvText(output.stream);
        cells[21] = UmiCsvText(output.text);
        status = UmiCsvDocumentAppendRow(document, cells, count);
    }
    if (status != UMI_STATUS_OK) { UmiCsvDocumentDestroy(document); return status; }
    *outDocument = document;
    return UMI_STATUS_OK;
}
