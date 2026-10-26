/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/ctest_comparison_file.h
 *
 * PURPOSE:
 *   Admit two complete saved reports through the shared bounded reader.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TESTING_CTEST_COMPARISON_FILE_H
#define UMICOM_TESTING_CTEST_COMPARISON_FILE_H
#include "umicom/testing/ctest_report_comparison.h"
#include "umicom/testing/ctest_report_file.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Read two absolute local JUnit paths synchronously; native hosts use a worker.
 * The result owns its changed records after both input reports are released.
 * Each read is bounded by UMI_CTEST_COLLECTION_MAX_BYTES. No file is written,
 * process launched, or checkout inspected. Reads occur sequentially, so this
 * does not promise an atomic snapshot of two files being rewritten elsewhere.
 * Prefer completed, immutable run reports. out becomes NULL on any failure. */
UmiStatus UmiCtestReportCompareFiles(const char *before_path, const char *after_path,
    UmiCtestReportComparison **out);

typedef struct UmiCtestReportComparisonIdentity {
    UmiCtestReportFileIdentity before;
    UmiCtestReportFileIdentity after;
} UmiCtestReportComparisonIdentity;

/* Optional identities are committed together only after both imports and
 * comparison succeed. Failure leaves identity unchanged and out NULL. NULL
 * identity preserves the original un-hashed reader. Each digest covers the
 * bytes used for that side; it does not prove an atomic pair or source revision.
 * The caller owns the returned value and must not overlap it with out. */
UmiStatus UmiCtestReportCompareFilesWithIdentity(const char *before_path, const char *after_path,
    UmiCtestReportComparison **out, UmiCtestReportComparisonIdentity *identity);
#ifdef __cplusplus
}
#endif
#endif
