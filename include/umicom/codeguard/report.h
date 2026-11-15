/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/report.h
 *
 * PURPOSE:
 *   Write CodeGuard findings as human-readable text, JSON or SARIF.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_REPORT_H
#define UMICOM_CODEGUARD_REPORT_H
#include <stdio.h>
#include "umicom/base/status.h"
#include "umicom/codeguard/result.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named code guard report format values accepted by this public contract.
 */
typedef enum UmiCodeGuardReportFormat {
    UMI_CODEGUARD_REPORT_TEXT = 0,
    UMI_CODEGUARD_REPORT_JSON = 1,
    UMI_CODEGUARD_REPORT_SARIF = 2
} UmiCodeGuardReportFormat;
/**
 * Write codeguard report in its stable representation and report capacity or input
 * failures to the caller.
 */
/* The result must remain alive and unchanged for the entire call. Validate all
 * fixed text fields as terminated UTF-8, known severity/category and confidence
 * 0..100 before writing any bytes. Invalid input leaves the stream untouched.
 * A successful call flushes but never closes the caller's stream. I/O failure
 * can leave a partial report; OK is not a disk durability guarantee. */
UmiStatus umi_codeguard_report_write(FILE *stream, UmiCodeGuardReportFormat format,
                                     const UmiCodeGuardResult *result);
/**
 * Provide the codeguard report file operation used by this module and its client
 * applications.
 */
/* Validate and render completely before opening the destination. Platform
 * provides UTF-8 Windows paths and checked write/close. Existing files are
 * overwritten; missing parents are not created. This is not an atomic replace:
 * an I/O error can leave a partial file. Relative destinations use current cwd. */
UmiStatus umi_codeguard_report_file(const char *path, UmiCodeGuardReportFormat format,
                                    const UmiCodeGuardResult *result);
/* Create a complete owned UTF-8 projection for a native view or exporter.
 * max_bytes bounds payload bytes, excluding the trailing NUL. Outputs remain
 * unchanged on failure, including capacity and allocation errors. Free a
 * successful allocation with report_free. The caller keeps result stable.
 * Text retains plain-text controls; JSON escapes them. SARIF uses encoded URI
 * paths and omits unknown locations/zero line and column values. */
UmiStatus umi_codeguard_report_render(UmiCodeGuardReportFormat format,
    const UmiCodeGuardResult *result, size_t max_bytes, char **out_text, size_t *out_size);
void umi_codeguard_report_free(char *text);
/* Save to a new absolute native path without replacing an existing leaf.
 * Validate the complete stable result and measure serialized bytes before
 * creating the destination. max_bytes bounds that payload; no complete report
 * allocation is needed. Parent directories must already exist and be trusted.
 * UmiOutputFile path policy applies (including Windows UTF-8 ordinary drives,
 * no device/UNC/alternate-stream names, and no dot segments).
 *
 * Creation is exclusive, not an atomic replacement. Readers may observe a
 * partial newly created report. Write/flush/close failures retain that file
 * for diagnosis, preserve the first failure, and never delete prior evidence.
 * Calls are synchronous and may block on storage; GUI hosts should schedule
 * an owned stable report on a worker. Existing report_file keeps its separate
 * overwrite behavior. No directory creation or existing-file deletion occurs. */
UmiStatus umi_codeguard_report_file_new(const char *path,
    UmiCodeGuardReportFormat format, const UmiCodeGuardResult *result, size_t max_bytes);
#ifdef __cplusplus
}
#endif
#endif
