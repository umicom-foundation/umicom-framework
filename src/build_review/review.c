/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build_review/review.c
 *
 * PURPOSE:
 *   Keep recorded operation outcomes separate from diagnostic parsing and filtering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/review.h"
#include "umicom/build/parser.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiBuildReview {
    UmiBuildResult *results;
    size_t count, omitted;
    UmiBuildReviewOrigin origin;
};

static int Terminated(const char *text, size_t capacity)
{
    return memchr(text, 0, capacity) != NULL;
}
static int ValidResult(const UmiBuildResult *result)
{
    if (result->phase < UMI_BUILD_PHASE_CONFIGURE || result->phase > UMI_BUILD_PHASE_DEPLOY ||
        result->state < UMI_BUILD_STATE_CREATED || result->state > UMI_BUILD_STATE_TIMED_OUT ||
        result->status < UMI_STATUS_OK || result->status > UMI_STATUS_BUSY ||
        !Terminated(result->profile_id, sizeof result->profile_id) ||
        !Terminated(result->command, sizeof result->command) ||
        !Terminated(result->output, sizeof result->output) ||
        result->diagnostics.count > UMI_BUILD_MAX_DIAGNOSTICS) return 0;
    for (size_t i = 0U; i < result->diagnostics.count; ++i) {
        const UmiBuildDiagnostic *d = &result->diagnostics.items[i];
        if (d->severity < UMI_BUILD_DIAGNOSTIC_NOTE || d->severity > UMI_BUILD_DIAGNOSTIC_FATAL ||
            !Terminated(d->file, sizeof d->file) || !Terminated(d->code, sizeof d->code) ||
            !Terminated(d->message, sizeof d->message) || d->message[0] == '\0' ||
            (d->line == 0U && d->column != 0U)) return 0;
    }
    return 1;
}
static UmiStatus Allocate(size_t count, UmiBuildReview **outReview)
{
    UmiBuildReview *review = calloc(1U, sizeof *review);
    if (review == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    review->results = calloc(count, sizeof *review->results);
    if (review->results == NULL) { free(review); return UMI_STATUS_OUT_OF_MEMORY; }
    review->origin = UMI_BUILD_REVIEW_RECORDED;
    *outReview = review;
    return UMI_STATUS_OK;
}
UmiStatus UmiBuildReviewCapture(const UmiBuildHistory *history,
    size_t maximumRecords, UmiBuildReview **outReview)
{
    UmiBuildReview *review = NULL;
    UmiStatus status;
    if (outReview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReview = NULL;
    if (history == NULL || maximumRecords == 0U || maximumRecords > UMI_BUILD_REVIEW_MAX_RECORDS)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = Allocate(maximumRecords, &review);
    if (status != UMI_STATUS_OK) return status;
    status = UmiBuildHistoryCopyRecent(history, review->results, maximumRecords,
        &review->count, &review->omitted);
    if (status == UMI_STATUS_OK) {
        for (size_t i = 0U; i < review->count; ++i) {
            if (!ValidResult(&review->results[i])) { status = UMI_STATUS_INVALID_STATE; break; }
        }
    }
    if (status != UMI_STATUS_OK) { UmiBuildReviewDestroy(review); return status; }
    *outReview = review;
    return UMI_STATUS_OK;
}
UmiStatus UmiBuildReviewFromResult(const UmiBuildResult *result, UmiBuildReview **outReview)
{
    UmiBuildReview *review = NULL;
    UmiStatus status;
    if (outReview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReview = NULL;
    if (result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!ValidResult(result)) return UMI_STATUS_INVALID_STATE;
    status = Allocate(1U, &review);
    if (status != UMI_STATUS_OK) return status;
    review->results[0] = *result;
    review->count = 1U;
    *outReview = review;
    return UMI_STATUS_OK;
}
UmiStatus UmiBuildReviewImportLog(const char *text, size_t length, UmiBuildReview **outReview)
{
    UmiBuildReview *review = NULL;
    UmiStatus status;
    if (outReview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReview = NULL;
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length >= UMI_BUILD_OUTPUT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(text, 0, length) != NULL) return UMI_STATUS_PARSE_ERROR;
    status = Allocate(1U, &review);
    if (status != UMI_STATUS_OK) return status;
    memcpy(review->results[0].output, text, length);
    review->results[0].output[length] = '\0';
    status = umi_build_parse_output(review->results[0].output, &review->results[0].diagnostics);
    if (status != UMI_STATUS_OK) { UmiBuildReviewDestroy(review); return status; }
    review->count = 1U;
    review->origin = UMI_BUILD_REVIEW_IMPORTED_LOG;
    *outReview = review;
    return UMI_STATUS_OK;
}
void UmiBuildReviewDestroy(UmiBuildReview *review)
{
    if (review != NULL) { free(review->results); free(review); }
}
size_t UmiBuildReviewCount(const UmiBuildReview *review) { return review != NULL ? review->count : 0U; }
size_t UmiBuildReviewOmitted(const UmiBuildReview *review) { return review != NULL ? review->omitted : 0U; }
static int ValidFilter(const UmiBuildReviewFilter *filter)
{
    return filter == NULL || (filter->minimumSeverity >= UMI_BUILD_DIAGNOSTIC_NOTE &&
        filter->minimumSeverity <= UMI_BUILD_DIAGNOSTIC_FATAL &&
        Terminated(filter->contains, sizeof filter->contains));
}
static int Matches(const UmiBuildDiagnostic *d, const UmiBuildReviewFilter *filter)
{
    return filter == NULL || (d->severity >= filter->minimumSeverity &&
        (filter->contains[0] == '\0' || strstr(d->file, filter->contains) != NULL ||
         strstr(d->code, filter->contains) != NULL || strstr(d->message, filter->contains) != NULL));
}
static UmiBuildReviewOutcome Outcome(const UmiBuildResult *result)
{
    switch (result->state) {
        case UMI_BUILD_STATE_CREATED:
        case UMI_BUILD_STATE_RUNNING: return UMI_BUILD_REVIEW_NOT_FINISHED;
        case UMI_BUILD_STATE_SUCCEEDED:
            return result->status == UMI_STATUS_OK && result->exit_code == 0
                ? UMI_BUILD_REVIEW_SUCCEEDED : UMI_BUILD_REVIEW_INCONSISTENT;
        case UMI_BUILD_STATE_FAILED:
            return result->status != UMI_STATUS_OK || result->exit_code != 0
                ? UMI_BUILD_REVIEW_FAILED : UMI_BUILD_REVIEW_INCONSISTENT;
        case UMI_BUILD_STATE_CANCELLED:
            return result->status == UMI_STATUS_CANCELLED
                ? UMI_BUILD_REVIEW_CANCELLED : UMI_BUILD_REVIEW_INCONSISTENT;
        case UMI_BUILD_STATE_TIMED_OUT:
            return result->status == UMI_STATUS_TIMEOUT
                ? UMI_BUILD_REVIEW_TIMED_OUT : UMI_BUILD_REVIEW_INCONSISTENT;
        default: return UMI_BUILD_REVIEW_INCONSISTENT;
    }
}
const char *UmiBuildReviewOutcomeText(UmiBuildReviewOutcome outcome)
{
    switch (outcome) {
        case UMI_BUILD_REVIEW_UNRECORDED: return "Unrecorded (imported text only)";
        case UMI_BUILD_REVIEW_NOT_FINISHED: return "Not finished";
        case UMI_BUILD_REVIEW_SUCCEEDED: return "Recorded success";
        case UMI_BUILD_REVIEW_FAILED: return "Recorded failure";
        case UMI_BUILD_REVIEW_CANCELLED: return "Recorded cancellation";
        case UMI_BUILD_REVIEW_TIMED_OUT: return "Recorded timeout";
        default: return "Inconsistent outcome fields";
    }
}
UmiStatus UmiBuildReviewSummarise(const UmiBuildReview *review, size_t index,
    const UmiBuildReviewFilter *filter, UmiBuildReviewSummary *outSummary)
{
    UmiBuildReviewSummary summary = {0};
    const UmiBuildResult *r;
    if (review == NULL || outSummary == NULL || !ValidFilter(filter)) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= review->count) return UMI_STATUS_NOT_FOUND;
    r = &review->results[index];
    summary.origin = review->origin;
    summary.outcome = review->origin == UMI_BUILD_REVIEW_IMPORTED_LOG
        ? UMI_BUILD_REVIEW_UNRECORDED : Outcome(r);
    summary.operationId = r->operation_id;
    summary.outputBytes = strlen(r->output);
    summary.totalDiagnostics = r->diagnostics.count;
    summary.droppedDiagnostics = r->diagnostics.dropped;
    for (size_t i = 0U; i < r->diagnostics.count; ++i) {
        const UmiBuildDiagnostic *d = &r->diagnostics.items[i];
        if (!Matches(d, filter)) continue;
        ++summary.visibleDiagnostics;
        switch (d->severity) {
            case UMI_BUILD_DIAGNOSTIC_NOTE: ++summary.notes; break;
            case UMI_BUILD_DIAGNOSTIC_WARNING: ++summary.warnings; break;
            case UMI_BUILD_DIAGNOSTIC_ERROR: ++summary.errors; break;
            case UMI_BUILD_DIAGNOSTIC_FATAL: ++summary.fatals; break;
        }
    }
    *outSummary = summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiBuildReviewDiagnostic(const UmiBuildReview *review, size_t index,
    const UmiBuildReviewFilter *filter, size_t visibleIndex, UmiBuildDiagnostic *outDiagnostic)
{
    if (review == NULL || outDiagnostic == NULL || !ValidFilter(filter)) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= review->count) return UMI_STATUS_NOT_FOUND;
    const UmiBuildDiagnosticList *list = &review->results[index].diagnostics;
    for (size_t i = 0U; i < list->count; ++i) {
        if (!Matches(&list->items[i], filter)) continue;
        if (visibleIndex-- == 0U) { *outDiagnostic = list->items[i]; return UMI_STATUS_OK; }
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Report generation is bounded independently from the renderer. The first pass
 * counts bytes, the second emits the same immutable content. No partial report
 * escapes on allocation/capacity failure. UI markup and terminal controls are
 * never interpreted; valid printable UTF-8 is retained as ordinary text. */
typedef struct Writer { char *text; size_t used, capacity; int failed; } Writer;
static void Bytes(Writer *w, const char *text, size_t length)
{
    if (w->failed) return;
    if (length > 8U * 1024U * 1024U - w->used) { w->failed = 1; return; }
    if (w->text != NULL) {
        if (length >= w->capacity - w->used) { w->failed = 1; return; }
        memcpy(w->text + w->used, text, length);
    }
    w->used += length;
}
static void Plain(Writer *w, const char *text) { Bytes(w, text, strlen(text)); }
static size_t Utf8Printable(const unsigned char *s, size_t left)
{
    size_t n; uint32_t cp, minimum;
    if (s[0] < 0x80U) return s[0] >= 0x20U && s[0] != 0x7fU ? 1U : 0U;
    if (s[0] >= 0xc2U && s[0] <= 0xdfU) { n = 2U; cp = s[0] & 0x1fU; minimum = 0x80U; }
    else if (s[0] >= 0xe0U && s[0] <= 0xefU) { n = 3U; cp = s[0] & 0x0fU; minimum = 0x800U; }
    else if (s[0] >= 0xf0U && s[0] <= 0xf4U) { n = 4U; cp = s[0] & 7U; minimum = 0x10000U; }
    else return 0U;
    if (left < n) return 0U;
    for (size_t i = 1U; i < n; ++i) {
        if ((s[i] & 0xc0U) != 0x80U) return 0U;
        cp = (cp << 6U) | (s[i] & 0x3fU);
    }
    if (cp < minimum || cp > 0x10ffffU || (cp >= 0xd800U && cp <= 0xdfffU) ||
        (cp >= 0x80U && cp <= 0x9fU) || cp == 0x061cU || cp == 0x200eU || cp == 0x200fU ||
        (cp >= 0x2028U && cp <= 0x202eU) || (cp >= 0x2066U && cp <= 0x2069U)) return 0U;
    return n;
}
static void Safe(Writer *w, const char *text, int multiline)
{
    static const char hex[] = "0123456789ABCDEF";
    size_t length = strlen(text);
    for (size_t i = 0U; i < length;) {
        unsigned char b = (unsigned char)text[i];
        if (multiline && (b == '\n' || b == '\t')) { Bytes(w, text + i++, 1U); continue; }
        size_t n = Utf8Printable((const unsigned char *)text + i, length - i);
        if (n != 0U) { Bytes(w, text + i, n); i += n; }
        else { char escaped[] = {'\\', 'x', hex[b >> 4U], hex[b & 15U]}; Bytes(w, escaped, 4U); ++i; }
    }
}
static const char *Phase(UmiBuildPhase phase)
{
    static const char *const names[] = {"Configure", "Build", "Test", "Clean", "Run", "Install", "Package", "Rebuild", "Deploy"};
    return names[(size_t)phase];
}
static void Emit(const UmiBuildReview *review, size_t index,
    const UmiBuildReviewFilter *filter, const UmiBuildReviewSummary *summary, Writer *writer)
{
    char numbers[512];
    const UmiBuildResult *r = &review->results[index];
    Plain(writer, "Umicom build review\n===================\n");
    Plain(writer, "Outcome: "); Plain(writer, UmiBuildReviewOutcomeText(summary->outcome)); Plain(writer, "\n");
    if (review->origin == UMI_BUILD_REVIEW_RECORDED) {
        (void)snprintf(numbers, sizeof numbers, "Operation: %" PRIu64 "\nPhase: %s\nExit code: %d\nStatus: %s\nDuration: %" PRIu64 " ms\nProfile: ",
            r->operation_id, Phase(r->phase), r->exit_code, umi_status_text(r->status), r->duration_ms);
        Plain(writer, numbers); Safe(writer, r->profile_id, 0);
        Plain(writer, "\nRecorded command (display only): "); Safe(writer, r->command, 0); Plain(writer, "\n");
    } else Plain(writer, "No process was observed by this import. Text is not an exit status.\n");
    Plain(writer, "Source revision: unavailable\nOriginal working directory: unavailable\nOutput completeness: unknown\n");
    (void)snprintf(numbers, sizeof numbers,
        "Diagnostics: %zu visible / %zu retained; %zu dropped\nVisible severities: %zu notes, %zu warnings, %zu errors, %zu fatal\nCaptured output: %zu bytes\nOlder retained history records omitted: %zu\n",
        summary->visibleDiagnostics, summary->totalDiagnostics, summary->droppedDiagnostics,
        summary->notes, summary->warnings, summary->errors, summary->fatals,
        summary->outputBytes, review->omitted);
    Plain(writer, numbers);
    Plain(writer, "Earlier history evictions: unknown. This review is not a certification.\n");
    Plain(writer, "Filtering changes this view only, not the recorded outcome.\n\nProblems\n--------\n");
    for (size_t i = 0U; i < r->diagnostics.count; ++i) {
        const UmiBuildDiagnostic *d = &r->diagnostics.items[i];
        static const char *const names[] = {"note", "warning", "error", "fatal"};
        if (!Matches(d, filter)) continue;
        Plain(writer, names[(size_t)d->severity]); Plain(writer, " | ");
        Safe(writer, d->file[0] != '\0' ? d->file : "(no source path)", 0);
        (void)snprintf(numbers, sizeof numbers, ":%zu:%zu | ", d->line, d->column);
        Plain(writer, numbers); Safe(writer, d->code, 0); Plain(writer, " | ");
        Safe(writer, d->message, 0); Plain(writer, "\n");
    }
    Plain(writer, "\nCaptured output (control/invalid bytes escaped)\n----------------------------------------------\n");
    Safe(writer, r->output, 1); Plain(writer, "\n");
}
UmiStatus UmiBuildReviewRender(const UmiBuildReview *review, size_t index,
    const UmiBuildReviewFilter *filter, char **outText, size_t *outLength)
{
    UmiBuildReviewSummary summary;
    UmiStatus status;
    Writer first = {0}, second = {0};
    if (outText != NULL) *outText = NULL;
    if (outLength != NULL) *outLength = 0U;
    if (outText == NULL || outLength == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiBuildReviewSummarise(review, index, filter, &summary);
    if (status != UMI_STATUS_OK) return status;
    Emit(review, index, filter, &summary, &first);
    if (first.failed) return UMI_STATUS_CAPACITY_EXCEEDED;
    second.capacity = first.used + 1U;
    second.text = malloc(second.capacity);
    if (second.text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    Emit(review, index, filter, &summary, &second);
    if (second.failed || second.used != first.used) { free(second.text); return UMI_STATUS_INTERNAL_ERROR; }
    second.text[second.used] = '\0';
    *outText = second.text; *outLength = second.used;
    return UMI_STATUS_OK;
}
void UmiBuildReviewTextFree(char *text) { free(text); }
