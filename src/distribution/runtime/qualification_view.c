/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_view.c
 * Purpose: Copy run evidence once so frontends never borrow a mutable worker ledger.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/distribution/runtime/qualification_view.h"
#include "umicom/distribution/runtime/qualification_report.h"
#include "qualification_internal.h"
#include <stdlib.h>
#include <string.h>
struct UmiQualificationView {
    UmiQualificationViewSummary summary;
    UmiQualificationViewRow *rows;
    char *names;
};
void UmiQualificationViewDestroy(UmiQualificationView *view)
{
    if (view == NULL) return;
    free(view->rows); free(view->names); free(view);
}
UmiStatus UmiQualificationViewCapture(const UmiReleaseQualification *run, UmiQualificationView **outView)
{
    if (run == NULL || outView == NULL || *outView != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const size_t nameLimit = 64U * 1024U * 1024U;
    size_t names = 0U;
    if (run->count > SIZE_MAX / sizeof(UmiQualificationViewRow) - 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < run->count; ++i) {
        size_t size = strlen(run->items[i].name) + 1U;
        if (size > nameLimit - names) return UMI_STATUS_CAPACITY_EXCEEDED;
        names += size;
    }
    UmiQualificationView *view = calloc(1U, sizeof(*view));
    if (view == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    view->rows = calloc(run->count + 2U, sizeof(*view->rows));
    view->names = malloc(names != 0U ? names : 1U);
    if (view->rows == NULL || view->names == NULL) {
        UmiQualificationViewDestroy(view); return UMI_STATUS_OUT_OF_MEMORY;
    }
    view->summary.identity = run->identity;
    view->summary.selection = run->selection;
    memcpy(view->summary.source, run->source, sizeof(view->summary.source));
    view->summary.rows = run->count + 2U;
    UmiStatus status = UmiReleaseQualificationDescribe(run, &view->summary.results);
    if (status != UMI_STATUS_OK) { UmiQualificationViewDestroy(view); return status; }
    view->rows[0] = (UmiQualificationViewRow){.index = 0U, .kind = UMI_QUALIFICATION_VIEW_BUILD,
        .name = "Build", .recorded = run->buildRecorded, .result = run->build};
    view->rows[1] = (UmiQualificationViewRow){.index = 1U, .kind = UMI_QUALIFICATION_VIEW_TEST_PROCESS,
        .name = "Test process", .recorded = run->testProcessRecorded, .result = run->testProcess};
    char *cursor = view->names;
    for (size_t i = 0U; i < run->count; ++i) {
        const UmiReleaseImpactItem *item = &run->items[i];
        UmiQualificationViewRow *row = &view->rows[i + 2U];
        size_t length = strlen(item->name) + 1U;
        memcpy(cursor, item->name, length); row->name = cursor; cursor += length;
        row->index = i + 2U; row->reasons = item->reasons;
        row->kind = item->kind == UMI_RELEASE_INVENTORY_TEST ? UMI_QUALIFICATION_VIEW_TEST : UMI_QUALIFICATION_VIEW_TARGET;
        if (row->kind == UMI_QUALIFICATION_VIEW_TEST) {
            row->recorded = run->tests[i - run->testStart].recorded;
            row->result = run->tests[i - run->testStart].result;
        }
    }
    *outView = view;
    return UMI_STATUS_OK;
}
UmiStatus UmiQualificationViewDescribe(const UmiQualificationView *view, UmiQualificationViewSummary *outSummary)
{
    if (view == NULL || outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSummary = view->summary; return UMI_STATUS_OK;
}
const UmiQualificationViewRow *UmiQualificationViewAt(const UmiQualificationView *view, size_t index)
{
    return view != NULL && index < view->summary.rows ? &view->rows[index] : NULL;
}
static bool Selected(const UmiQualificationViewRow *row, UmiQualificationViewFilter filter)
{
    switch (filter) {
    case UMI_QUALIFICATION_VIEW_ALL: return true;
    case UMI_QUALIFICATION_VIEW_PROBLEMS:
        return row->recorded && row->result.outcome != UMI_QUALIFICATION_PASSED;
    case UMI_QUALIFICATION_VIEW_MISSING:
        return !row->recorded && (row->kind == UMI_QUALIFICATION_VIEW_TEST || row->kind == UMI_QUALIFICATION_VIEW_BUILD);
    case UMI_QUALIFICATION_VIEW_TESTS: return row->kind == UMI_QUALIFICATION_VIEW_TEST;
    case UMI_QUALIFICATION_VIEW_TARGETS: return row->kind == UMI_QUALIFICATION_VIEW_TARGET;
    }
    return false;
}
UmiStatus UmiQualificationViewPage(const UmiQualificationView *view, UmiQualificationViewFilter filter,
    size_t offset, UmiQualificationViewRow *rows, size_t capacity, size_t *outCount, size_t *outTotal)
{
    if (view == NULL || outCount == NULL || outTotal == NULL || outCount == outTotal ||
        (unsigned)filter > UMI_QUALIFICATION_VIEW_TARGETS || capacity > UMI_QUALIFICATION_VIEW_PAGE_LIMIT ||
        (capacity != 0U && rows == NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t total = 0U;
    for (size_t i = 0U; i < view->summary.rows; ++i)
        if (Selected(&view->rows[i], filter)) ++total;
    if (offset > total) return UMI_STATUS_INVALID_ARGUMENT;
    size_t skipped = 0U, count = 0U;
    for (size_t i = 0U; i < view->summary.rows && count < capacity; ++i) {
        if (!Selected(&view->rows[i], filter)) continue;
        if (skipped++ < offset) continue;
        rows[count++] = view->rows[i];
    }
    *outCount = count; *outTotal = total;
    return UMI_STATUS_OK;
}
const char *UmiQualificationViewStateText(const UmiQualificationView *view)
{
    if (view == NULL) return "No qualification record";
    if (view->summary.savedReport) {
        if (!view->summary.results.closed) return "Saved incomplete attempt";
        return view->summary.results.readyForOwnerReview ? "Saved scoped result: ready for review" : "Saved scoped result: blocked";
    }
    if (!view->summary.results.closed) return "Run in progress";
    return view->summary.results.readyForOwnerReview ? "Ready for scoped review" : "Review blocked";
}
const char *UmiQualificationViewRowStateText(const UmiQualificationViewRow *row)
{
    if (row == NULL) return "Unknown";
    if (row->kind == UMI_QUALIFICATION_VIEW_TARGET) return "Selected target";
    if (!row->recorded)
        return row->kind == UMI_QUALIFICATION_VIEW_TEST_PROCESS ? "Not supplied" : "Missing result";
    return UmiReleaseQualificationOutcomeText(row->result.outcome);
}

/* A parsed historical report remains visibly distinct from a worker snapshot.
 * This marker is private so frontends cannot accidentally remove provenance. */
void UmiQualificationViewMarkSaved(UmiQualificationView *view)
{
    if (view != NULL) view->summary.savedReport = true;
}
