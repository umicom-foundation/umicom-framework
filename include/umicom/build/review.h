/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/review.h
 *
 * PURPOSE:
 *   Publish immutable, read-only reviews of existing build-history records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_REVIEW_H
#define UMICOM_BUILD_REVIEW_H
#include "umicom/build/history.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BUILD_REVIEW_MAX_RECORDS 16U
#define UMI_BUILD_REVIEW_FILTER_CAPACITY 256U

typedef struct UmiBuildReview UmiBuildReview;
typedef enum UmiBuildReviewOrigin {
    UMI_BUILD_REVIEW_RECORDED = 1,
    UMI_BUILD_REVIEW_IMPORTED_LOG = 2
} UmiBuildReviewOrigin;
typedef enum UmiBuildReviewOutcome {
    UMI_BUILD_REVIEW_UNRECORDED = 0,
    UMI_BUILD_REVIEW_NOT_FINISHED = 1,
    UMI_BUILD_REVIEW_SUCCEEDED = 2,
    UMI_BUILD_REVIEW_FAILED = 3,
    UMI_BUILD_REVIEW_CANCELLED = 4,
    UMI_BUILD_REVIEW_TIMED_OUT = 5,
    UMI_BUILD_REVIEW_INCONSISTENT = 6
} UmiBuildReviewOutcome;
typedef struct UmiBuildReviewFilter {
    UmiBuildDiagnosticSeverity minimumSeverity;
    /* Case-sensitive literal bytes in file, code or message; never a regex. */
    char contains[UMI_BUILD_REVIEW_FILTER_CAPACITY];
} UmiBuildReviewFilter;
typedef struct UmiBuildReviewSummary {
    UmiBuildReviewOrigin origin;
    UmiBuildReviewOutcome outcome;
    uint64_t operationId;
    size_t totalDiagnostics, visibleDiagnostics, droppedDiagnostics, outputBytes;
    size_t notes, warnings, errors, fatals;
    /* Source revision, original working directory and output completeness are
     * absent from the older result ABI. Do not infer them from today's profile. */
    int sourceRevisionKnown, workingDirectoryKnown, outputCompletenessKnown;
} UmiBuildReviewSummary;

/* Capture at most maximumRecords (1..16), newest retained records in chronological
 * order. Uses the history's mutex once. outReview is cleared before any failure.
 * The history must survive this call, not the review. Each record is validated
 * before publication. No threads/processes, files, database or source edits. */
UmiStatus UmiBuildReviewCapture(const UmiBuildHistory *history,
    size_t maximumRecords, UmiBuildReview **outReview);
/* Copy one already recorded result. The supplied record must stay immutable
 * during the call. Useful for worker completion handlers on their owner thread. */
UmiStatus UmiBuildReviewFromResult(const UmiBuildResult *result,
    UmiBuildReview **outReview);
/* Import an explicitly sized, NUL-free log (0..65535 bytes). Its recorded
 * outcome is UNRECORDED, regardless of text saying "success" or "failed".
 * Parsing is the existing Framework build parser, not a second grammar. */
UmiStatus UmiBuildReviewImportLog(const char *text, size_t length,
    UmiBuildReview **outReview);
void UmiBuildReviewDestroy(UmiBuildReview *review);
size_t UmiBuildReviewCount(const UmiBuildReview *review);
/* Older records omitted by capture while still retained in the source history.
 * Records already evicted from that history are unknown and are not counted. */
size_t UmiBuildReviewOmitted(const UmiBuildReview *review);
UmiStatus UmiBuildReviewSummarise(const UmiBuildReview *review, size_t index,
    const UmiBuildReviewFilter *filter, UmiBuildReviewSummary *outSummary);
/* Copy the nth matching diagnostic without exposing mutable review storage. */
UmiStatus UmiBuildReviewDiagnostic(const UmiBuildReview *review, size_t index,
    const UmiBuildReviewFilter *filter, size_t visibleIndex,
    UmiBuildDiagnostic *outDiagnostic);
/* Allocate a complete UTF-8 text report, including captured output. Control,
 * invalid UTF-8 and bidi-format bytes are visibly escaped; text never executes.
 * The result is NUL-terminated; outLength excludes it. Free with TextFree.
 * No partial result is returned on error. All callbacks/UI use copied storage.
 * Concurrent readers are permitted; destruction requires them to have finished. */
UmiStatus UmiBuildReviewRender(const UmiBuildReview *review, size_t index,
    const UmiBuildReviewFilter *filter, char **outText, size_t *outLength);
void UmiBuildReviewTextFree(char *text);
const char *UmiBuildReviewOutcomeText(UmiBuildReviewOutcome outcome);
#ifdef __cplusplus
}
#endif
#endif
