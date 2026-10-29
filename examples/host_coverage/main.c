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
#include <umicom/platform/native_arguments.h>
#include <stdlib.h>

#include <umicom/testing/ctest_coverage_view.h>
#include <string.h>

/* Preserve the full-review consumer. The replacement uses an immutable view
 * so display filters cannot narrow the qualification decision. */
#if 0
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

#endif

/* Retain the original entry declaration for review. Framework captures
 * native Unicode arguments before these UTF-8 file APIs consume paths. */
#if 0
int main(int argc, char **argv)
#endif
static int ConsumerMain(int argc, char **argv)
{
    if (argc < 3 || argc > 5 ||
        (argc >= 4 && strcmp(argv[3], "--outstanding") && strcmp(argv[3], "--passed"))) {
        fprintf(stderr, "Usage: host-coverage-consumer <absolute coverage.csv> <source identity> "
            "[--outstanding|--passed [literal test/capability text]]\n");
        return 2;
    }
    UmiCtestCoverageFilter filter;
    UmiCtestCoverageFilterInit(&filter);
    filter.outcomes = argc < 4 ? UMI_CTEST_COVERAGE_VIEW_ALL :
        !strcmp(argv[3], "--passed") ? 1U : UMI_CTEST_COVERAGE_VIEW_OUTSTANDING;
    if (argc == 5) {
        if (strlen(argv[4]) >= sizeof filter.text_contains) {
            fprintf(stderr, "Coverage filter exceeds its text limit.\n"); return 2;
        }
        memcpy(filter.text_contains, argv[4], strlen(argv[4]) + 1U);
    }
    UmiStatus status = UmiCtestCoverageFilterValidate(&filter);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Coverage filter refused: %s\n", umi_status_text(status)); return 2;
    }
    UmiCtestCoverageReview *review = NULL;
    status = UmiCtestCoverageReviewReadFile(argv[1], argv[2], &review);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Coverage input refused: %s\n", umi_status_text(status)); return 2;
    }
    UmiCtestCoverageView *view = NULL;
    status = UmiCtestCoverageViewCreate(review, &filter, &view);
    UmiCtestCoverageViewSummary summary;
    if (status == UMI_STATUS_OK) status = UmiCtestCoverageViewRead(view, &summary);
    size_t offset = 0U;
    /* The index borrows the immutable review. Page it without changing its
     * qualification decision, and release the view before its owning review. */
    while (status == UMI_STATUS_OK) {
        size_t bytes = 0U;
        status = UmiCtestCoverageViewFormat(view, offset, 64U, NULL, 0U, &bytes);
        char *text = status == UMI_STATUS_OK ? malloc(bytes + 1U) : NULL;
        if (status == UMI_STATUS_OK && text == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
        if (status == UMI_STATUS_OK)
            status = UmiCtestCoverageViewFormat(view, offset, 64U, text, bytes + 1U, &bytes);
        if (status == UMI_STATUS_OK && fwrite(text, 1U, bytes, stdout) != bytes)
            status = UMI_STATUS_IO_ERROR;
        free(text);
        if (status != UMI_STATUS_OK || summary.matches - offset <= 64U) break;
        offset += 64U;
    }
    if (status == UMI_STATUS_OK && fflush(stdout) != 0) status = UMI_STATUS_IO_ERROR;
    int result = 2;
    if (status == UMI_STATUS_OK)
        result = UmiCtestCoverageRequirePassed(review) == UMI_STATUS_OK ? 0 : 1;
    else fprintf(stderr, "Coverage output failed: %s\n", umi_status_text(status));
    UmiCtestCoverageViewDestroy(view);
    UmiCtestCoverageReviewDestroy(review);
    return result;
}

int main(int argc, char **argv)
{
    int result = 2;
    UmiStatus status = UmiNativeArgumentsDispatch(argc, argv, ConsumerMain, &result);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Cannot capture native arguments: %s\n", umi_status_text(status));
        return 2;
    }
    return result;
}
