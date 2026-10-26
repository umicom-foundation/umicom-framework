/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/report_comparison/main.c
 *
 * PURPOSE:
 *   Compare completed reports using only installed public Framework contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <umicom/testing/ctest_comparison_file.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: report-comparison-consumer <absolute baseline.xml> <absolute current.xml>\n");
        return 2;
    }
    UmiCtestReportComparison *comparison = NULL;
    UmiStatus status = UmiCtestReportCompareFiles(argv[1], argv[2], &comparison);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Comparison input refused: %s\n", umi_status_text(status));
        return 2;
    }
    UmiCtestReportComparisonSummary summary;
    status = UmiCtestReportComparisonReadSummary(comparison, &summary);
    size_t offset = 0;
    /* The immutable comparison owns names after both temporary reports are
     * destroyed. Pagination bounds text allocation without discarding rows. */
    while (status == UMI_STATUS_OK) {
        size_t required = 0;
        status = UmiCtestReportComparisonFormat(comparison, offset, 64U, NULL, 0, &required);
        char *text = status == UMI_STATUS_OK ? malloc(required + 1U) : NULL;
        if (status == UMI_STATUS_OK && !text) status = UMI_STATUS_OUT_OF_MEMORY;
        if (status == UMI_STATUS_OK)
            status = UmiCtestReportComparisonFormat(comparison, offset, 64U, text, required + 1U, &required);
        if (status == UMI_STATUS_OK && fwrite(text, 1U, required, stdout) != required)
            status = UMI_STATUS_IO_ERROR;
        free(text);
        if (status != UMI_STATUS_OK || summary.changed - offset <= 64U) break;
        offset += 64U;
    }
    if (status == UMI_STATUS_OK && fflush(stdout) != 0) status = UMI_STATUS_IO_ERROR;
    if (status != UMI_STATUS_OK)
        fprintf(stderr, "Comparison output failed: %s\n", umi_status_text(status));
    UmiCtestReportComparisonDestroy(comparison);
    /* This example's exit status reports input/output success, never whether
     * a source revision passed tests. Unchanged failures remain failures. */
    return status == UMI_STATUS_OK ? 0 : 2;
}
