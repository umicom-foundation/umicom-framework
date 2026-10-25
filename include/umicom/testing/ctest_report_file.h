/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/ctest_report_file.h
 *
 * PURPOSE:
 *   Read an explicitly selected saved CTest JUnit file through the bounded Framework input service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_CTEST_REPORT_FILE_H
#define UMICOM_TESTING_CTEST_REPORT_FILE_H
#include "umicom/testing/ctest_report.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Absolute local path only, at most UMI_CTEST_COLLECTION_MAX_BYTES.
 * Synchronous; call on a worker in native applications. No process launch,
 * repository inspection or source freshness assertion. out becomes NULL on
 * failure. The complete input is copied before parsing; the file may change
 * later without changing the imported report. */
UmiStatus UmiCtestReportReadFile(const char *path, UmiCtestReport **out);
#ifdef __cplusplus
}
#endif
#endif
