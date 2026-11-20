/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/boot_report_file.h
 *
 * PURPOSE:
 *   Load a bounded captured boot report while preserving output on rejection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_BOOT_REPORT_FILE_H
#define UMICOM_PLATFORM_BOOT_REPORT_FILE_H
#include "umicom/base/sha256.h"
#include "umicom/platform/boot_report.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Read a selected absolute regular file under the boot-report byte bound.
 * Reuses UmiInputFileRead: no current-directory lookup, file write or launch.
 * Parent aliases may be followed; select a trusted parent directory. Reads may
 * block, so interactive callers should use their existing worker service.
 *
 * expectedSourceId is optional. When supplied it must be a lowercase SHA-256
 * string, and a different parsed identity returns INVALID_STATE. Neither that
 * comparison nor the report authenticates a publisher or proves current health.
 * Any failure preserves *outReport. No input pointer is retained. */
UmiStatus UmiBootReportLoad(const char *path, const char *expectedSourceId,
    UmiBootReport *outReport);

/* Consumers can conditionally support exact capture identity when compiling
 * against an older Framework without this additive file inspection API. */
#define UMI_BOOT_REPORT_HAS_CAPTURE_IDENTITY 1

typedef struct UmiBootReportCapture {
    UmiBootReport report;
    size_t byteCount;
    char captureSha256[UMI_SHA256_HEX_CAPACITY];
} UmiBootReportCapture;

/* Parse and hash the same bounded native read. captureSha256 includes every
 * file byte, including line endings and the final LF. expectedCaptureSha256
 * is optional and uses the same lowercase form as expectedSourceId. A
 * mismatched source or capture identity returns INVALID_STATE. The complete
 * output remains unchanged on failure. A digest establishes exact byte
 * identity when compared with a reviewed value. It grants no boot authority. */
UmiStatus UmiBootReportInspectFile(const char *path, const char *expectedSourceId,
    const char *expectedCaptureSha256, UmiBootReportCapture *outCapture);
#ifdef __cplusplus
}
#endif
#endif
