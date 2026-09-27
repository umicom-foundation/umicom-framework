/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/build_review/cli.c
 *
 * PURPOSE:
 *   Inspect an explicitly selected build log without executing any command it contains.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/review.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    UmiBuildReview *review = NULL;
    char *text = NULL, *report = NULL;
    size_t length = 0U, reportLength = 0U;
    UmiStatus status;
    if (argc == 2 && strcmp(argv[1], "--self-test") == 0) {
        UmiBuildReviewSummary summary;
        status = UmiBuildReviewImportLog("ordinary output", 15U, &review);
        if (status == UMI_STATUS_OK) status = UmiBuildReviewSummarise(review, 0U, NULL, &summary);
        UmiBuildReviewDestroy(review);
        if (status != UMI_STATUS_OK || summary.outcome != UMI_BUILD_REVIEW_UNRECORDED) return 1;
        puts("Native review self-test passed. No file or process was opened.");
        return 0;
    }
    if (argc != 3 || strcmp(argv[1], "log") != 0) {
        puts("Umicom build review\nUsage: umicom-build-review log <plain-text-file>\n       umicom-build-review --self-test\nThe tool reads at most 65535 bytes. No command in the log is executed.");
        return argc == 2 && strcmp(argv[1], "--help") == 0 ? 0 : 2;
    }
    text = malloc(UMI_BUILD_OUTPUT_CAPACITY);
    if (text == NULL) return 1;
    FILE *file = fopen(argv[2], "rb");
    if (file == NULL) { free(text); fputs("Could not open the selected log.\n", stderr); return 1; }
    length = fread(text, 1U, UMI_BUILD_OUTPUT_CAPACITY, file);
    int readFailed = ferror(file);
    int closeFailed = fclose(file);
    status = readFailed || closeFailed != 0 ? UMI_STATUS_IO_ERROR :
        UmiBuildReviewImportLog(text, length, &review);
    free(text);
    if (status == UMI_STATUS_OK) status = UmiBuildReviewRender(review, 0U, NULL, &report, &reportLength);
    if (status == UMI_STATUS_OK && fwrite(report, 1U, reportLength, stdout) != reportLength) status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK && fflush(stdout) != 0) status = UMI_STATUS_IO_ERROR;
    UmiBuildReviewTextFree(report);
    UmiBuildReviewDestroy(review);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Log inspection failed: %s\n", umi_status_text(status));
    return status == UMI_STATUS_OK ? 0 : 1;
}
