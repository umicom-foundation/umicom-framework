/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/host_coverage/main.c
 *
 * PURPOSE:
 *   Review a saved host plan through installed public Framework APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <umicom/testing/ctest_coverage.h>
#include <umicom/testing/ctest_coverage_manifest.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: host-coverage-consumer <absolute coverage.csv> <source identity>\n");
        return 2;
    }
    UmiCtestCoverageReview *review = NULL;
    UmiStatus status = UmiCtestCoverageReviewReadFile(argv[1], argv[2], &review);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Coverage input refused: %s\n", umi_status_text(status));
        return 2;
    }
    UmiCtestCoverageSummary summary;
    status = UmiCtestCoverageReviewRead(review, &summary);
    size_t offset = 0;
    /* Pages bound temporary text memory independently of the complete review.
     * The review owns its source metadata and observations throughout both
     * formatting passes; no source or report files are changed by this example. */
    while (status == UMI_STATUS_OK) {
        size_t bytes = 0;
        status = UmiCtestCoverageReviewFormat(review, offset, 64U, NULL, 0, &bytes);
        char *text = status == UMI_STATUS_OK ? malloc(bytes + 1U) : NULL;
        if (status == UMI_STATUS_OK && !text) status = UMI_STATUS_OUT_OF_MEMORY;
        if (status == UMI_STATUS_OK)
            status = UmiCtestCoverageReviewFormat(review, offset, 64U, text, bytes + 1U, &bytes);
        if (status == UMI_STATUS_OK && fwrite(text, 1U, bytes, stdout) != bytes)
            status = UMI_STATUS_IO_ERROR;
        free(text);
        if (status != UMI_STATUS_OK || summary.requirements - offset <= 64U) break;
        offset += 64U;
    }
    if (status == UMI_STATUS_OK && fflush(stdout) != 0) status = UMI_STATUS_IO_ERROR;
    int result = 2;
    if (status == UMI_STATUS_OK)
        result = UmiCtestCoverageRequirePassed(review) == UMI_STATUS_OK ? 0 : 1;
    else fprintf(stderr, "Coverage output failed: %s\n", umi_status_text(status));
    UmiCtestCoverageReviewDestroy(review);
    return result;
}
