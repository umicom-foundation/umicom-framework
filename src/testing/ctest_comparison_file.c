/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_comparison_file.c
 *
 * PURPOSE:
 *   Own both admitted reports until comparison has copied the result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/testing/ctest_comparison_file.h"
#include "umicom/testing/ctest_report_file.h"
#include "umicom/platform/output_file.h"

UmiStatus UmiCtestReportCompareFiles(const char *before_path, const char *after_path,
    UmiCtestReportComparison **out)
{
    if (!out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    /* Validate both selections before beginning any I/O, so a bad second
     * argument cannot accidentally trigger a potentially expensive first read. */
    UmiStatus status = UmiOutputFileValidatePath(before_path);
    if (status == UMI_STATUS_OK) status = UmiOutputFileValidatePath(after_path);
    UmiCtestReport *before = NULL, *after = NULL;
    if (status == UMI_STATUS_OK) status = UmiCtestReportReadFile(before_path, &before);
    if (status == UMI_STATUS_OK) status = UmiCtestReportReadFile(after_path, &after);
    if (status == UMI_STATUS_OK) status = UmiCtestReportCompare(before, after, out);
    UmiCtestReportDestroy(after);
    UmiCtestReportDestroy(before);
    return status;
}
