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
#include "umicom/base/sha256.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Absolute local path only, at most UMI_CTEST_COLLECTION_MAX_BYTES.
 * Synchronous; call on a worker in native applications. No process launch,
 * repository inspection or source freshness assertion. out becomes NULL on
 * failure. The complete input is copied before parsing; the file may change
 * later without changing the imported report. An empty regular file returns
 * UMI_STATUS_PARSE_ERROR; invalid path arguments remain INVALID_ARGUMENT. */
UmiStatus UmiCtestReportReadFile(const char *path, UmiCtestReport **out);

/* Identifies the exact bytes passed to the parser, including whitespace and
 * line endings. This value owns its digest; it does not borrow the path or
 * report. It identifies content, not a signer, source revision or test host. */
typedef struct UmiCtestReportFileIdentity {
    size_t bytes;
    char sha256[UMI_SHA256_HEX_CAPACITY];
} UmiCtestReportFileIdentity;

/* Same admission and ownership contract as ReadFile. When identity is non-NULL,
 * calculate SHA-256 from the very same owned buffer used for parsing. Publish
 * identity only after import succeeds; every failure leaves it unchanged.
 * NULL identity avoids hashing. out and identity must not overlap. The caller
 * should select a completed report: reading a concurrently rewritten file is
 * not a filesystem snapshot, even though the digest always matches parsed bytes. */
UmiStatus UmiCtestReportReadFileWithIdentity(const char *path, UmiCtestReport **out,
    UmiCtestReportFileIdentity *identity);
#ifdef __cplusplus
}
#endif
#endif
