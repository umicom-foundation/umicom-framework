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
#ifdef __cplusplus
}
#endif
#endif
